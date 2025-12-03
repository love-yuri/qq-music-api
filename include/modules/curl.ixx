/*
 * @Author: love-yuri yuri2078170658@gmail.com
 * @Date: 2025-11-18 18:27:33
 * @LastEditTime: 2025-12-01 20:30:00
 * @Description: 优化后的 Curl 封装，支持 C++23
 */
module;

#include "yuri_log.hpp"
#include <curl/curl.h>
#include <memory>
#include <string>
#include <string_view>
#include <format>
#include <expected>
#include <optional>
#include "glaze/glaze.hpp"

export module curl;

/**
 * RAII 封装 curl_slist
 */
struct CurlSlistDeleter {
  void operator()(curl_slist *list) const noexcept {
    if (list) curl_slist_free_all(list);
  }
};
using CurlSlistPtr = std::unique_ptr<curl_slist, CurlSlistDeleter>;

/**
 * RAII 封装 curl escaped string
 */
struct CurlFreeDeleter {
  void operator()(char *ptr) const noexcept {
    if (ptr) curl_free(ptr);
  }
};

using CurlEscapedPtr = std::unique_ptr<char, CurlFreeDeleter>;

/**
 * 安全的 URL 编码
 */
std::optional<std::string> url_encode(CURL *curl, const std::string_view str) {
  const CurlEscapedPtr escaped (
    curl_easy_escape(curl, str.data(), static_cast<int>(str.length()))
  );

  if (!escaped) {
    return std::nullopt;
  }

  return std::string(escaped.get());
}

/**
 * 构建查询字符串
 */
std::optional<std::string> build_query_string(
  CURL *curl,
  const std::vector<std::pair<std::string_view, std::string_view>> &params
) {
  if (params.empty()) {
    return "";
  }

  std::string result;
  result.reserve(params.size() * 32); // 预分配空间

  for (size_t i = 0; i < params.size(); ++i) {
    const auto &[key, val] = params[i];

    auto encoded_key = url_encode(curl, key);
    auto encoded_val = url_encode(curl, val);

    if (!encoded_key || !encoded_val) {
      return std::nullopt;
    }

    if (i > 0) {
      result += "&";
    }
    result += std::format("{}={}", *encoded_key, *encoded_val);
  }

  return result;
}

/**
 * 构建 headers
 */
CurlSlistPtr build_headers(
  const std::vector<std::pair<std::string_view, std::string_view>> &headers_map,
  std::optional<std::string_view> content_type = std::nullopt
) {
  curl_slist *raw_headers = nullptr;

  // 添加用户自定义 headers
  for (const auto &[key, val] : headers_map) {
    std::string header = std::format("{}: {}", key, val);
    raw_headers = curl_slist_append(raw_headers, header.c_str());
  }

  // 添加 Content-Type（如果指定且用户未设置）
  if (content_type) {
    bool has_content_type = false;
    for (const auto &key : headers_map | std::views::keys) {
      if (key == "Content-Type" || key == "content-type") {
        has_content_type = true;
        break;
      }
    }

    if (!has_content_type) {
      const std::string ct = std::format("Content-Type: {}", *content_type);
      raw_headers = curl_slist_append(raw_headers, ct.c_str());
    }
  }

  return CurlSlistPtr(raw_headers);
}

/**
 * Curl 请求错误类型
 */
export enum class CurlError {
  InitFailed,
  PerformFailed,
  EncodeError,
  InvalidParameter
};

export class Curl final {
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl_{nullptr, curl_easy_cleanup};
  std::string response_data_{}; // 响应数据

  // 回调函数，用于接收数据
  static size_t write_callback(const char *contents, size_t size, size_t nmemb, Curl *request) {
    const size_t total_size = size * nmemb;
    try {
      request->response_data_.append(contents, total_size);
      return total_size;
    } catch (...) {
      return 0; // 出错时返回 0，curl 会终止传输
    }
  }

public:
  explicit Curl() = default;

  /**
   * 初始化操作
   */
  [[nodiscard]] bool init() {
    curl_.reset(curl_easy_init());
    if (!curl_) {
      yerror << "curl 初始化失败!";
      return false;
    }

    // 设置回调函数
    curl_easy_setopt(curl_.get(), CURLOPT_WRITEFUNCTION, &Curl::write_callback);
    curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, this);

    // 设置通用选项
    curl_easy_setopt(curl_.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_.get(), CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl_.get(), CURLOPT_SSL_VERIFYPEER, 1L); // 验证 SSL 证书
    curl_easy_setopt(curl_.get(), CURLOPT_SSL_VERIFYHOST, 2L);

    return true;
  }

  /**
   * 执行请求
   * @return 成功返回响应数据，失败返回错误
   */
  [[nodiscard]] std::expected<std::string, CurlError> commit() {
    response_data_.clear(); // 清空旧数据

    if (const auto res = curl_easy_perform(curl_.get()); res != CURLE_OK) {
      yerror << "请求失败: " << curl_easy_strerror(res);
      return std::unexpected(CurlError::PerformFailed);
    }

    return std::move(response_data_);
  }

  /**
   * @return curl 指针
   */
  [[nodiscard]] CURL *get() const noexcept {
    return curl_.get();
  }

  // 禁止拷贝，允许移动
  Curl(const Curl &) = delete;
  Curl &operator=(const Curl &) = delete;
  Curl(Curl &&) noexcept = default;
  Curl &operator=(Curl &&) noexcept = default;
};

export namespace curl {

using KeyValueList = std::vector<std::pair<std::string_view, std::string_view>>;

/**
 * GET 请求
 * @param url URL
 * @param headers_map 请求头
 * @param params_map 查询参数
 * @return 成功返回响应数据，失败返回错误
 */
[[nodiscard]] std::expected<std::string, CurlError> get(
  const std::string_view url,
  const KeyValueList &headers_map = {},
  const KeyValueList &params_map = {}
) {
  Curl curl_ptr;
  if (!curl_ptr.init()) {
    return std::unexpected(CurlError::InitFailed);
  }

  const auto curl = curl_ptr.get();
  std::string url_str(url);

  // 构建查询参数
  if (!params_map.empty()) {
    const auto query = build_query_string(curl, params_map);
    if (!query) {
      return std::unexpected(CurlError::EncodeError);
    }

    url_str += (url_str.find('?') == std::string::npos ? "?" : "&") + *query;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url_str.c_str());

  // 设置 headers
  if (!headers_map.empty()) {
    const auto headers = build_headers(headers_map);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
  }

  return curl_ptr.commit();
}

/**
 * POST 请求（原始数据）
 * @param url URL
 * @param data POST 数据
 * @param headers_map 请求头
 * @param content_type Content-Type（默认为 application/x-www-form-urlencoded）
 * @return 成功返回响应数据，失败返回错误
 */
[[nodiscard]] std::expected<std::string, CurlError> post(
  const std::string_view url,
  const std::string_view data,
  const KeyValueList &headers_map = {},
  std::string_view content_type = "application/x-www-form-urlencoded"
) {
  Curl curl_ptr;
  if (!curl_ptr.init()) {
    return std::unexpected(CurlError::InitFailed);
  }

  const auto curl = curl_ptr.get();

  curl_easy_setopt(curl, CURLOPT_URL, url.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(data.size()));

  // 设置 headers
  const auto headers = build_headers(headers_map, content_type);
  if (headers) {
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.get());
  }

  return curl_ptr.commit();
}

/**
 * POST 请求（表单数据）
 * @param url URL
 * @param form 表单数据
 * @param headers_map 请求头
 * @return 成功返回响应数据，失败返回错误
 */
[[nodiscard]] std::expected<std::string, CurlError> post_form(
  const std::string_view url,
  const KeyValueList &form,
  const KeyValueList &headers_map = {}
) {
  Curl curl_ptr;
  if (!curl_ptr.init()) {
    return std::unexpected(CurlError::InitFailed);
  }

  const auto curl = curl_ptr.get();

  // 构建表单数据
  const auto post_data = build_query_string(curl, form);
  if (!post_data) {
    return std::unexpected(CurlError::EncodeError);
  }

  return post(url, *post_data, headers_map, "application/x-www-form-urlencoded");
}

/**
 * POST JSON 请求
 * @param url URL
 * @param json_data JSON 字符串
 * @param headers_map 请求头
 * @return 成功返回响应数据，失败返回错误
 */
[[nodiscard]] std::expected<std::string, CurlError> post_json(
  const std::string_view url,
  const std::string_view json_data,
  const KeyValueList &headers_map = {}
) {
  return post(url, json_data, headers_map, "application/json");
}

} // namespace curl