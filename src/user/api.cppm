/*
 * @Description: 用户 API 内部实现
 */
module;
#include "core/json_utils.hpp"
module qq_music_api:user_detail;

import std;
import :curl;
import :config;
import :user_result;

namespace qqmusic_api::user::detail {

/**
 * 获取当前登录用户信息
 * @return 当前登录用户信息
 */
UserInfoResult get_user_info() {
  constexpr std::string_view url = "https://c6.y.qq.com/rsc/fcgi-bin/fcg_get_profile_homepage.fcg?_=1783559424029&cv=4747474&ct=24&format=json&inCharset=utf-8&outCharset=utf-8&notice=0&platform=yqq.json&needNewCode=0&cid=205360838&reqfrom=1";
  const auto res = curl::get(
    url,
    {{"referer", "https://y.qq.com/n/ryqq_v2/profile"},
     {"cookie", qqmusic_api_config.cookie}});

  return read_json<UserInfoResult>(res.value());
}

} // namespace qqmusic_api::user::detail
