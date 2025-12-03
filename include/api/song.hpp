#pragma once

#include <string_view>
#include "yuri_log.hpp"
#include "utils/json_utils.hpp"

import qqmusic.song_result;
import global_config;
import nodejs;
import curl;

namespace qqmusic_api {

/**
 * 歌单操作的baseapi
 * @param sign_json
 */
inline std::string song_api_base(const std::string_view sign_json) {
  if (global_config.qq.empty() || global_config.cookie.empty()) {
    throw std::runtime_error(std::format("{} 需要用户qq以及cookie!", __FUNCTION__));
  }

  constexpr std::string_view base_url = "https://u6.y.qq.com/cgi-bin/musics.fcg?_=1763902346841&encoding=ag-1&sign=";
  const auto url = std::string(base_url) + nodejs::get_sign(sign_json);
  const auto sign_data = nodejs::get_encrypt(sign_json);
  const curl::KeyValueList headers = {
    {"cookie", global_config.cookie},
    {"accept", "application/octet-stream"},
    {"accept-language", "zh-CN,zh;q=0.9,en;q=0.8"},
    {"sec-ch-ua-mobile", "?0"},
    {"user-agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/142.0.0.0 Safari/537.36"},
  };
  return nodejs::get_decrypt(curl::post(url, sign_data, headers).value());
}

inline std::string get_song_download_url(const std::string_view mid, const SongFileFormat &format = m4a_format) {
  if (global_config.qq.empty() || global_config.cookie.empty()) {
    throw std::runtime_error(std::format("{} 需要用户qq以及cookie!", __FUNCTION__));
  }

  constexpr std::string_view base_url = "https://u6.y.qq.com/cgi-bin/musics.fcg?_=1764590219737&encoding=ag-1&sign=";
  constexpr auto sign_data_json = R"(
    {{
      "comm": {{
        "cv": 4747474,
        "ct": 24,
        "format": "json",
        "inCharset": "utf-8",
        "outCharset": "utf-8",
        "notice": 0,
        "platform": "yqq.json",
        "needNewCode": 1,
        "g_tk_new_20200303": 2050245758,
        "g_tk": 2050245758
      }},
      "req_1": {{
        "module": "music.vkey.GetEVkey",
        "method": "GetUrl",
        "param": {{
          "filename": ["{0}"],
          "guid": "5737980864",
          "songmid": [
              "{1}"
          ],
          "songtype": [1],
          "loginflag": 1,
          "platform": "20",
          "xcdn": 1
        }}
      }}
    }}
  )";

  const auto filename = std::format("{}{}.{}", format.s, mid, format.e);
  const auto sign_json = std::format(sign_data_json, filename, mid);
  const auto url = std::string(base_url) + nodejs::get_sign(sign_json);
  const auto sign_data = nodejs::get_encrypt(sign_json);
  const curl::KeyValueList headers = {
    {"cookie", global_config.cookie},
  };

  const auto res = nodejs::get_decrypt(curl::post(url, sign_data, headers).value());
  if (const auto [sip, midurlinfo] = read_json<SongDownloadUrlResult>(res).req_1.data; !sip.empty() && !midurlinfo.empty()) {
    return sip.front() + midurlinfo.front().purl;
  }

  return res;
}

} // namespace qqmusic_api