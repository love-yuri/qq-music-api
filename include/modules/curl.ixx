/*
 * @Author: love-yuri yuri2078170658@gmail.com
 * @Date: 2025-11-18 18:27:33
 * @LastEditTime: 2025-11-18 19:21:40
 * @Description:
 */
module;

#include "yuri_log.hpp"
#include <curl/curl.h>
#include <memory>
#include <string>
#include <string_view>
#include <format>
#include "glaze/glaze.hpp"

export module curl;

/**
 * 构建get请求参数
 * @param params 参数
 * @return encoded后的字符串
 */
std::string build_query_string(const std::vector<std::pair<std::string_view, std::string_view>> &params) {
  std::ostringstream oss;
  bool first = true;
  for (const auto &[key, val] : params) {
    if (!first) {
      oss << "&";
    }
    first = false;
    oss << curl_easy_escape(nullptr, key.data(), static_cast<int>(key.length()))
        << "="
        << curl_easy_escape(nullptr, val.data(), static_cast<int>(val.length()));
  }
  return oss.str();
}

export class Curl final {
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl{nullptr, curl_easy_cleanup};

  std::string response_data{}; // 结果

  // 回调函数，用于接收数据
  static size_t WriteCallback(const char *contents, const size_t size, const size_t nmemb, Curl *request) {
    const size_t total_size = size * nmemb;
    request->response_data.append(contents, total_size);
    return total_size;
  }

public:
  explicit Curl() = default;

  // 初始化操作
  bool init() {
    curl.reset(curl_easy_init());
    if (!curl) {
      yerror << "curl 初始化失败!";
      return false;
    }

    // 设置回调函数
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &Curl::WriteCallback);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, this);

    // 设置一些通用选项
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L); // 跟随重定向
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 30L);       // 30秒超时

    return true;
  }

  /**
   * 调用请求并获取返回值
   * @return 接收的字符串
   */
  std::string &commit() {
    if (const auto res = curl_easy_perform(curl.get()); res != CURLE_OK) {
      yerror << "请求失败: " << curl_easy_strerror(res);
    }

    return response_data;
  }

  /**
   * @return curl指针
   */
  [[nodiscard]] CURL *get() const {
    return curl.get();
  }

  // 禁止拷贝，允许移动
  Curl(const Curl &) = delete;
  Curl &operator=(const Curl &) = delete;
  Curl(Curl &&) = default;
  Curl &operator=(Curl &&) = default;
};

export namespace curl {

using KeyValueList = std::vector<std::pair<std::string_view, std::string_view>>;

/**
 * get请求
 * @param url url
 * @param headers_map headers
 * @param params_map params
 */
std::string get(const std::string_view url, const KeyValueList &headers_map = {}, const KeyValueList &params_map = {}) {
  Curl curl_ptr;
  if (not curl_ptr.init()) {
    return {};
  }

  const auto curl = curl_ptr.get();

  std::string url_str(url);

  if (!params_map.empty()) {
    const auto query = build_query_string(params_map);
    // 判断是否已有 ?
    if (url_str.find('?') == std::string::npos)
      url_str += "?" + query;
    else
      url_str += "&" + query;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url_str.c_str());

  // header
  std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers{
    nullptr,
    curl_slist_free_all,
  };

  if (!headers_map.empty()) {
    curl_slist *raw_headers = nullptr;
    for (auto &[k, v] : headers_map) {
      std::string h = std::format("{}: {}", k, v);
      raw_headers = curl_slist_append(raw_headers, h.c_str());
    }

    // 设置 content-type
    raw_headers = curl_slist_append(raw_headers, "Content-Type: application/x-www-form-urlencoded");

    headers.reset(raw_headers);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
  }

  return curl_ptr.commit();
}

/**
 * post请求
 * @param url url
 * @param data post data
 */
std::string post(const std::string_view url, const std::string_view data) {
  Curl curl_ptr;
  if (not curl_ptr.init()) {
    return {};
  }

  const auto curl = curl_ptr.get();

  curl_easy_setopt(curl, CURLOPT_URL, url.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

  return curl_ptr.commit();
}

/**
 * post请求
 * @param url url
 * @param data post data
 * @param headers_map header
 */
std::string post(const std::string_view url, const std::string_view data, const KeyValueList &headers_map) {
  Curl curl_ptr;
  if (not curl_ptr.init()) {
    return {};
  }
  const auto curl = curl_ptr.get();

  // ----设置 url ----
  curl_easy_setopt(curl, CURLOPT_URL, url.data());

  // ----设置 data ----
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

  // ----设置 Headers ----
  std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers{
    nullptr,
    curl_slist_free_all,
  };

  if (!headers_map.empty()) {
    curl_slist *raw_headers = nullptr;
    for (auto &[k, v] : headers_map) {
      std::string h = std::format("{}: {}", k, v);
      raw_headers = curl_slist_append(raw_headers, h.c_str());
    }

    // 设置 content-type
    raw_headers = curl_slist_append(raw_headers, "Content-Type: application/x-www-form-urlencoded");

    headers.reset(raw_headers);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
  }

  return curl_ptr.commit();
}

/**
 * 发起post请求
 * @param url url
 * @param form 表单数据
 * @param headers_map 请求头
 */
std::string post(const std::string_view url, const KeyValueList &form, const KeyValueList &headers_map) {
  Curl curl_ptr;
  if (not curl_ptr.init()) {
    return {};
  }

  const auto curl = curl_ptr.get();

  // ---- 1. 拼接 POST 表单 ----
  std::string post_data;
  for (auto &[k, v] : form) {
    if (!post_data.empty()) {
      post_data += "&";
    }
    post_data += curl_easy_escape(curl, k.data(), 0);
    post_data += "=";
    post_data += curl_easy_escape(curl, v.data(), 0);
  }

  // ---- 2. 设置 URL ----
  curl_easy_setopt(curl, CURLOPT_URL, std::string(url).c_str());

  // ---- 3. 设置 POST 数据 ----
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data.size());

  // ---- 4. 设置 Headers ----
  std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers{
    nullptr,
    curl_slist_free_all,
  };

  if (!headers_map.empty()) {
    curl_slist *raw_headers = nullptr;
    for (auto &[k, v] : headers_map) {
      std::string h = std::format("{}: {}", k, v);
      raw_headers = curl_slist_append(raw_headers, h.c_str());
    }

    // 设置 content-type
    raw_headers = curl_slist_append(raw_headers, "Content-Type: application/x-www-form-urlencoded");

    headers.reset(raw_headers);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
  }

  return curl_ptr.commit();
}
} // namespace curl
