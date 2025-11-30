#pragma once

#include "yuri_log.hpp"
#include "utils/json_utils.hpp"

import qqmusic.play_list_result;
import curl;
import global_config;
import nodejs;

/**
 * 获取用户收藏的歌单，私密歌单需要传递, 使用需要配置qq
 * 如果要获取私人歌单，需要配置 cookie
 * @param size 获取的数量默认11
 */
inline UserPlaylistsResult get_user_playlists(int size = 11) {
  constexpr auto baseUrl = "https://c6.y.qq.com/rsc/fcgi-bin/fcg_user_created_diss?r=1763983092962&_=1763983092962&cv=4747474&ct=24&format=json&inCharset=utf-8&outCharset=utf-8&notice=0&platform=yqq.json&needNewCode=1&uin={0}&g_tk_new_20200303=549478032&g_tk=549478032&hostuin={0}&sin=0&size={1}";
  const auto res = curl::get(
  std::format(baseUrl, global_config.qq, size),
  {
      { "referer", "https://y.qq.com/" },
      { "cookie", global_config.cookie }
    }
  );
  return read_json<UserPlaylistsResult>(res);
}

/**
 * 歌单操作的baseapi
 * @param sign_json
 */
inline std::string playlist_api_base(const std::string_view sign_json) {
  if (global_config.qq.empty() || global_config.cookie.empty()) {
    throw std::runtime_error(std::format("{} 需要用户qq以及cookie!", __FUNCTION__));
  }

  constexpr std::string_view base_url = "https://u6.y.qq.com/cgi-bin/musics.fcg?_=1763902346841&encoding=ag-1&sign=";
  const auto url = std::string(base_url) + nodejs::get_sign(sign_json);
  const auto sign_data = nodejs::get_encrypt(sign_json);
  const curl::KeyValueList headers = {
    { "cookie", global_config.cookie },
    { "accept", "application/octet-stream" },
    { "accept-language", "zh-CN,zh;q=0.9,en;q=0.8" },
    { "sec-ch-ua-mobile", "?0" },
    { "user-agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/142.0.0.0 Safari/537.36" },
  };
  return nodejs::get_decrypt(curl::post(url, sign_data, headers));
}


/**
 * 将歌曲收藏到歌单里，强制要求qq和cookie
 * 单次请求耗时约 270ms
 * @param dir_id 歌单id，使用dirId
 * @param song_id 歌曲id
 * @return 是否添加成功
 */
inline bool add_song_to_playlist(const int dir_id, const std::uint64_t song_id) {
  if (global_config.qq.empty() || global_config.cookie.empty()) {
    throw std::runtime_error(std::format("{} 需要用户qq以及cookie!", __FUNCTION__));
  }
  constexpr std::string_view sign_data_json = R"(
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
        "module": "music.musicasset.PlaylistDetailWrite",
        "method": "AddSonglist",
        "param": {{
          "dirId": {0},
          "v_songInfo": [
            {{
              "songType": 0,
              "songId": {1}
            }}
          ]
        }}
      }}
    }}
  )";

  const auto sign_json = std::format(sign_data_json, dir_id, song_id);
  const auto res = playlist_api_base(sign_json);

  // 返回size > 视为成功
  return res.size() > 200;
}

/**
 * 将歌曲从歌单里移除，强制要求qq和cookie
 * 单次请求耗时约 270ms
 * @param dir_id 歌单id，使用dirId
 * @param song_id 歌曲id
 * @return 是否移除成功
 */
inline bool delete_song_from_playlist(const int dir_id, const std::uint64_t song_id) {
  if (global_config.qq.empty() || global_config.cookie.empty()) {
    throw std::runtime_error(std::format("{} 需要用户qq以及cookie!", __FUNCTION__));
  }
  constexpr std::string_view sign_data_json = R"(
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
          "g_tk_new_20200303": 549478032,
          "g_tk": 549478032
      }},
      "req_1": {{
          "module": "music.musicasset.PlaylistDetailWrite",
          "method": "DelSonglist",
          "param": {{
              "dirId": {0},
              "v_songInfo": [{{
                  "songType": 0,
                  "songId": {1}
              }}]
          }}
      }}
    }}
  )";

  const auto sign_json = std::format(sign_data_json, dir_id, song_id);
  const auto res = playlist_api_base(sign_json);

  // 返回size > 视为成功
  return res.size() > 200;
}