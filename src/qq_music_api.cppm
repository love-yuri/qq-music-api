/*
 * @Description: 聚合模块 — re-export 所有子模块，提供 JSON 反序列化后的公开 API
 */
module;
#pragma GCC diagnostic ignored "-Wimport-implementation-partition-unit-in-interface-unit"
#include "core/json_utils.hpp"
export module qq_music_api;

export import :curl;
export import :config;
export import :nodejs;
export import :playlist_result;
export import :song_result;
export import :detail;

import :log;
import :detail;
import :playlist_detail;
import :song_detail;
import :album_detail;

export namespace qqmusic_api::playlist {

/**
 * 获取用户收藏的歌单，私密歌单需要传递，使用需要配置qq
 * 如果要获取私人歌单，需要配置 cookie
 * @param size 获取的数量默认11
 */
UserPlaylistsResult get_user_playlists(const int size = 11) {
  return read_json<UserPlaylistsResult>(detail::get_user_playlists(size));
}

/**
 * 将歌曲收藏到歌单里，强制要求qq和cookie
 * 单次请求耗时约 270ms
 * @param dir_id 歌单id，使用dirId
 * @param song_id 歌曲id
 * @return 是否添加成功
 */
bool add_song_to_playlist(const int dir_id, const std::uint64_t song_id) {
  return detail::add_song_to_playlist(dir_id, song_id).size() > 200;
}

/**
 * 将歌曲从歌单里移除，强制要求qq和cookie
 * 单次请求耗时约 270ms
 * @param dir_id 歌单id，使用dirId
 * @param song_id 歌曲id
 * @return 是否移除成功
 */
bool delete_song_from_playlist(const int dir_id, const std::uint64_t song_id) {
  return detail::delete_song_from_playlist(dir_id, song_id).size() > 200;
}

/**
 * 获取歌单详情
 * @param tid 歌单tid
 * @param begin 起始位置
 * @param size 获取数量
 */
UserPlaylistsDetailResult get_user_playlists_detail(const std::uint64_t tid, int begin = 0, int size = 20) {
  return read_json<UserPlaylistsDetailResult>(detail::get_user_playlists_detail(tid, begin, size));
}

} // namespace qqmusic_api::playlist

export namespace qqmusic_api::song {

/**
 * 获取歌曲的下载链接
 * @param mid 歌曲mid
 * @param format 目标格式，请确保歌曲有该音源
 * @return 歌曲下载链接，如果没找到则返回空
 */
std::string get_song_download_url(const std::string_view mid, const SongFileFormat &format = m4a_format) {
  return detail::resolve_song_download_url(mid, format);
}

/**
 * 下载已解析的歌曲 URL 到指定文件
 * @param url 歌曲下载 URL
 * @param path 输出文件路径
 * @return 下载成功时返回 true，否则返回 false
 */
bool download_song_file(const std::string_view url, const std::filesystem::path &path) {
  return detail::download_song_file(url, path);
}

} // namespace qqmusic_api::song

export namespace qqmusic_api::album {

/**
 * 获取专辑封面 URL
 * @param size 图片尺寸
 * @param album_mid 专辑 mid
 * @return 专辑封面 URL
 */
std::string get_album_cover_url(const int size, const std::string_view album_mid) {
  return detail::get_album_cover_url(size, album_mid);
}

/**
 * 获取专辑封面图片链接
 * @param size 图片尺寸
 * @param album_mid 专辑 mid
 * @return 图片链接
 */
std::string get_album_cover(const int size, const std::string_view album_mid) {
  return detail::get_album_cover(size, album_mid);
}

} // namespace qqmusic_api::album
