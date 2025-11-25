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

export module curl;

export class Curl {
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl{ nullptr, curl_easy_cleanup };

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

  // 禁止拷贝，允许移动
  Curl(const Curl &) = delete;
  Curl &operator=(const Curl &) = delete;
  Curl(Curl &&) = default;
  Curl &operator=(Curl &&) = default;
};