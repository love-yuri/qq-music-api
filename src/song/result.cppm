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

// 歌曲下载所需的基础信息
struct SongDownloadInfo {
  std::string mid;      // 歌曲 mid
  std::string title;    // 歌曲标题
  std::string artist;   // 歌手名
  bool has_flac{};      // 是否存在 flac 音源
  bool has_ape{};       // 是否存在 ape 音源
  bool has_mp3_320{};   // 是否存在 320k mp3 音源
  bool has_mp3_128{};   // 是否存在 128k mp3 音源
};

constexpr SongFileFormat m4a_format     = {"C40", "m4a"};
constexpr SongFileFormat mp3_128_format = {"M50", "mp3"};
constexpr SongFileFormat mp3_320_format = {"M80", "mp3"};
constexpr SongFileFormat ape_format     = {"A00", "ape"};
constexpr SongFileFormat flac_format    = {"F00", "flac"};

} // namespace qqmusic_api::song
