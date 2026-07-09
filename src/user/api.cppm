/*
 * @Description: 用户 API 内部实现（原始 HTTP 请求，不含 JSON 反序列化）
 */
module;
module qq_music_api:user_detail;

import std;
import :curl;
import :config;

namespace qqmusic_api::user::detail {

/**
 * 获取当前登录用户信息
 * @return 原始响应字符串
 */
std::string get_user_info() {
  constexpr std::string_view url = "https://c6.y.qq.com/rsc/fcgi-bin/fcg_get_profile_homepage.fcg?_=1783559424029&cv=4747474&ct=24&format=json&inCharset=utf-8&outCharset=utf-8&notice=0&platform=yqq.json&needNewCode=0&cid=205360838&reqfrom=1";
  const auto res = curl::get(
    url,
    {
      { "referer", "https://y.qq.com/n/ryqq_v2/profile" },
      { "cookie", qqmusic_api_config.cookie }
    }
  );

  return res.value();
}

} // namespace qqmusic_api::user::detail
