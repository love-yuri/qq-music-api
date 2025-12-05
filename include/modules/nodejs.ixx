module;
#include <string>
#include <string_view>
#include "yuri_log.hpp"
#include <format>
#include <filesystem>
export module nodejs;

namespace fs = std::filesystem;

/*
 * 提供操作nodejs的一些功能
 */
export namespace nodejs {

const auto temp_filepath = (fs::temp_directory_path() / "qqmusic_api_nodejs_temp_file").string();
constexpr std::string_view crypto_utils_path = "script/crypto_util.js";  // 默认加解密脚本位置

/**
 * 执行js脚本, 默认会在最后将temp_file的路径作为参数传递
 * @param cmd js指令
 * @return
 */
  std::string execute(const std::string_view cmd) {
#ifdef _WIN32
    // Windows: 使用 _popen 和 "w" 模式需要分开处理读写
    const std::string command = std::format("node {} {}", cmd, temp_filepath);
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(std::format("node {} {}", cmd, temp_filepath).c_str(), "r");
#endif
    if (!pipe) {
      yerror << "nodejs 启动失败";
      return {};
    }

    // 注意: popen 的 "r" 模式只能读取，不能写入
    // 如果需要向子进程写入数据，需要使用其他方法
    // std::fprintf(pipe, "yuri is yes");  // 这行在 "r" 模式下不工作
    // std::fflush(pipe);

    char buffer[1024];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
      result += buffer;
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
  // 写入参数
  (std::fstream(temp_filepath, std::ios::out) << data).flush();
  const auto cmd = std::format("{} sign", crypto_utils_path);
  return execute(cmd);
}

/**
 * 获取加密后的data
 * @param data 发送data
 */
std::string get_encrypt(const std::string_view data) {
  // 写入参数
  (std::fstream(temp_filepath, std::ios::out) << data).flush();
  const auto cmd = std::format("{} encrypt", crypto_utils_path);
  return execute(cmd);
}

/**
 * 将数据解密
 * @param data 发送data
 */
std::string get_decrypt(const std::string_view data) {
  // 写入参数
  (std::fstream(temp_filepath, std::ios::out) << data).flush();
  const auto cmd = std::format("{} decrypt", crypto_utils_path);
  return execute(cmd);
}

}