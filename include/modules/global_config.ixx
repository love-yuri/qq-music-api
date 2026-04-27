module;
#include "yuri_log.hpp"
#include <filesystem>
#include <fstream>
#include <string>
export module global_config;

class GlobalConfig final {
public:
  GlobalConfig() = default;
  ~GlobalConfig() = default;

  // 禁止拷贝和移动
  GlobalConfig(const GlobalConfig &) = delete;
  GlobalConfig &operator=(const GlobalConfig &) = delete;
  GlobalConfig(GlobalConfig &&) = delete;
  GlobalConfig &operator=(GlobalConfig &&) = delete;

  std::string qq{};       // 用户qq
  std::string cookie{};   // 用户cookie
  bool has_login = false; // 是否登录

  /**
   * 从文件中登录 默认使用 cookie.txt
   */
  void loginFromFile(const std::string_view path = "cookie.txt") {
    const auto size = std::filesystem::file_size(path);
    std::ifstream file(path.data(), std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error(std::format("Failed to open file: {}", path));
    }

    std::string content(size, '\0');
    file.read(content.data(), static_cast<long long>(size));
    cookie = content;
  }
};

export GlobalConfig global_config = {};