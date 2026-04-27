module;
#include <string>
#include <string_view>
#include "yuri_log.hpp"
#include <format>
#include <filesystem>
#include <fstream>
#include <thread>
#include <atomic>

export module nodejs;

namespace fs = std::filesystem;

/*
 * 提供操作nodejs的一些功能
 */
export namespace nodejs {

constexpr std::string_view crypto_utils_path = "script/crypto_util.js";  // 默认加解密脚本位置

/**
 * RAII 临时文件清理
 */
struct TempFileGuard {
  fs::path path;
  explicit TempFileGuard(fs::path p) : path(std::move(p)) {}
  ~TempFileGuard() {
    if (!path.empty()) {
      std::error_code ec;
      fs::remove(path, ec);
    }
  }
  TempFileGuard(const TempFileGuard&) = delete;
  TempFileGuard& operator=(const TempFileGuard&) = delete;
  TempFileGuard(TempFileGuard&&) = delete;
  TempFileGuard& operator=(TempFileGuard&&) = delete;
};

/**
 * 执行js脚本, 通过唯一临时文件传递数据
 * @param cmd js指令
 * @param data 传递给js的数据
 * @return
 */
std::string execute(const std::string_view cmd, const std::string_view data) {
  static std::atomic counter{0};
  const auto unique_path = (fs::temp_directory_path() / std::format(
    "qqmusic_node_{}_{}",
    std::hash<std::thread::id>{}(std::this_thread::get_id()),
    counter.fetch_add(1))).string();
  TempFileGuard guard(unique_path);

  {
    std::ofstream ofs(unique_path, std::ios::binary);
    if (!ofs) {
      yerror << "无法创建临时文件";
      return {};
    }
    ofs.write(data.data(), static_cast<std::streamsize>(data.size()));
    ofs.flush();
  }

  const auto quoted_path = std::format("\"{}\"", unique_path);
  const std::string command = std::format("node {} {}", cmd, quoted_path);

#ifdef _WIN32
  FILE* pipe = _popen(command.c_str(), "rb");
#else
  FILE* pipe = popen(command.c_str(), "r");
#endif
  if (!pipe) {
    yerror << "nodejs 启动失败";
    return {};
  }

  std::string result;
  char buffer[4096];
  size_t n = 0;
  while ((n = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
    result.append(buffer, n);
  }
#ifdef _WIN32
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  return result;
}

/**
 * 获取发送需要的sign
 * @param data 发送data
 */
std::string get_sign(const std::string_view data) {
  return execute(std::format("{} sign", crypto_utils_path), data);
}

/**
 * 获取加密后的data
 * @param data 发送data
 */
std::string get_encrypt(const std::string_view data) {
  return execute(std::format("{} encrypt", crypto_utils_path), data);
}

/**
 * 将数据解密
 * @param data 发送data
 */
std::string get_decrypt(const std::string_view data) {
  return execute(std::format("{} decrypt", crypto_utils_path), data);
}

}