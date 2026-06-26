/*
 * @Description: 全局配置单例（qq号、cookie、登录状态）
 */
module;
export module qq_music_api:config;

import std;
import :log;

namespace {

std::string trimCookiePart(std::string_view value) {
  while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
    value.remove_prefix(1);
  }
  while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
    value.remove_suffix(1);
  }
  return std::string(value);
}

std::string cookieValue(std::string_view cookie, std::string_view name) {
  std::size_t pos = 0;
  while (pos < cookie.size()) {
    const auto end = cookie.find(';', pos);
    const auto part = cookie.substr(pos, end == std::string_view::npos ? cookie.size() - pos : end - pos);
    const auto eq = part.find('=');
    if (eq != std::string_view::npos) {
      const auto key = trimCookiePart(part.substr(0, eq));
      if (key == name) {
        return trimCookiePart(part.substr(eq + 1));
      }
    }

    if (end == std::string_view::npos) {
      break;
    }
    pos = end + 1;
  }
  return {};
}

std::string normalizeQq(std::string value) {
  std::string qq;
  qq.reserve(value.size());
  for (const char ch : value) {
    if (std::isdigit(static_cast<unsigned char>(ch))) {
      qq.push_back(ch);
    }
  }
  return qq;
}

std::string qqFromCookie(std::string_view cookie) {
  for (const std::string_view name : {"uin", "p_uin", "ptui_loginuin"}) {
    if (auto value = normalizeQq(cookieValue(cookie, name)); !value.empty()) {
      return value;
    }
  }
  return {};
}

bool hasLoginCookieKey(std::string_view cookie) {
  for (const std::string_view name : {"qm_keyst", "qqmusic_key", "p_skey", "skey"}) {
    if (!cookieValue(cookie, name).empty()) {
      return true;
    }
  }
  return false;
}

} // namespace

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
    try
    {
      const auto size = std::filesystem::file_size(path);
      std::ifstream file(path.data(), std::ios::binary);
      if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", path));
      }

      std::string content(size, '\0');
      file.read(content.data(), static_cast<long long>(size));
      cookie = content;
      if (const auto parsed_qq = qqFromCookie(cookie); !parsed_qq.empty()) {
        qq = parsed_qq;
      }
      has_login = !cookie.empty() && !qq.empty() && hasLoginCookieKey(cookie);
    } catch (std::exception &e) {
      cookie.clear();
      has_login = false;
      qqmusic_api::error(std::format("从cookie文件登录失败, msg: {}, path: {}", e.what(), path));
    }
  }
};

export GlobalConfig qqmusic_api_config = {};

