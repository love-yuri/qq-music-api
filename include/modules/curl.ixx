/*
 * @Author: love-yuri yuri2078170658@gmail.com
 * @Date: 2025-11-18 18:27:33
 * @LastEditTime: 2025-11-18 19:21:40
 * @Description:
 */
module;

#include "../yuri_log.hpp"
#include <curl/curl.h>
#include <memory>
#include <string>
#include <string_view>
#include "glaze/glaze.hpp"

export module curl;

export class Curl {
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl{nullptr, curl_easy_cleanup};

  std::string response_data{};

  // 回调函数，用于接收数据
  static size_t WriteCallback(const char *contents, const size_t size, const size_t nmemb, Curl *request) {
    const size_t total_size = size * nmemb;
    request->response_data.append(contents, total_size);
    return total_size;
  }

  // 初始化操作
  bool init() {
    curl.reset(curl_easy_init());
    if (!curl) {
      yerror << "Failed to initialize curl!";
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

public:
  Curl() {
    init();
  }

  /**
   * 调用请求并获取返回值
   * @return 接收的字符串
   */
  std::string &commit() {
    if (const auto res = curl_easy_perform(curl.get()); res != CURLE_OK) {
      yerror << "GET请求失败: " << curl_easy_strerror(res);
    }

    return response_data;
  }

  /**
   * @return curl指针
   */
  CURL *get() {
    return curl.get();
  }

  // 禁止拷贝，允许移动
  Curl(const Curl &) = delete;
  Curl &operator=(const Curl &) = delete;
  Curl(Curl &&) = default;
  Curl &operator=(Curl &&) = default;
};

export namespace curl {

using Header = std::vector<std::pair<std::string_view, std::string_view>>;

std::string get(const std::string_view url, const Header &headers_map = {}) {
  Curl curl_ptr;
  const auto curl = curl_ptr.get();

  curl_easy_setopt(curl, CURLOPT_URL, url.data());

  // header
  std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers {
    nullptr,
    curl_slist_free_all
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