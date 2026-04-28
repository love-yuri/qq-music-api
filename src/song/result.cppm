/*
 * @Description: 歌曲 API 返回值结构体及文件格式定义
 */
module;
export module qq_music_api:song_result;

import std;
import :detail;

export namespace qqmusic_api::song {

// 歌曲下载url返回值
struct SongDownloadUrlResult {
  SongReq1Type req_1;
};

// 歌曲文件格式
struct SongFileFormat {
  std::string_view s; // 前缀，如 "C400"
  std::string_view e; // 扩展名，如 ".m4a"
};

constexpr SongFileFormat m4a_format     = {"C40", "m4a"};
constexpr SongFileFormat mp3_128_format = {"M50", "mp3"};
constexpr SongFileFormat mp3_320_format = {"M80", "mp3"};
constexpr SongFileFormat ape_format     = {"A00", "ape"};
constexpr SongFileFormat flac_format    = {"F00", "flac"};

} // namespace qqmusic_api::song
