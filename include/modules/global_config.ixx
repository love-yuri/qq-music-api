module;
#include "yuri_log.hpp"
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
};

export GlobalConfig global_config = {};