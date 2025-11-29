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
  FILE* pipe = popen(std::format("node {} {}", cmd, temp_filepath).c_str(), "r");
  if (!pipe) {
    yerror << "nodejs 启动失败";
    return {};
  }

  std::fprintf(pipe, "yuri is yes");
  std::fflush(pipe);

  char buffer[1024];
  std::string result;
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }

  pclose(pipe);
  return result;
}

/**
 * 获取发送需要的sign
 * @param data 发送data
 */
std::string get_sign(const std::string_view data) {
  // 写入参数/**/
  (std::fstream(temp_filepath, std::ios::out) << data).flush();
  const auto cmd = std::format("{} sign", crypto_utils_path);
  return execute(cmd);
}

}