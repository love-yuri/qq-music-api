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
 * 查找歌曲本地缓存文件
 * @param song 歌曲下载信息
 * @param cache_dir 缓存目录，默认 musics
 * @return 已存在的缓存文件路径，未命中时返回空路径
 */
std::filesystem::path cached_song_path(const SongDownloadInfo &song, const std::filesystem::path &cache_dir = "musics") {
  return detail::cached_song_path(song, cache_dir);
}

/**
 * 下载歌曲文件到本地缓存
 * @param song 歌曲下载信息
 * @param cache_dir 缓存目录，默认 musics
 * @return 可播放的本地文件路径，失败时返回空路径
 */
std::filesystem::path download_song_file(const SongDownloadInfo &song, const std::filesystem::path &cache_dir = "musics") {
  return detail::download_song_file(song, cache_dir);
}

} // namespace qqmusic_api::song
