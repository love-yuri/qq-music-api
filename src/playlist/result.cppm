/*
 * @Description: 歌单 API 返回值结构体
 */
module;
export module qq_music_api:playlist_result;

import std;
import :detail;

export namespace qqmusic_api::playlist {

// 用户歌单详情
struct UserPlaylistsDetailResult {
  int code{};
  PlaylistReq1Type req_1;
};

// 用户歌单列表
struct UserPlaylistsResult {
  int code;
  std::string message;
  PlaylistDataType data;
};

} // namespace qqmusic_api::playlist
