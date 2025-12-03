module;
#include <string>
#include <vector>
export module qqmusic.song_result;

struct MidurlinfoType {
  std::string filename;
  std::string purl;
};

struct DataType {
  std::vector<std::string>    sip;
  std::vector<MidurlinfoType> midurlinfo;
};

struct Req1Type {
  int      code{};
  DataType data;
};



export namespace qqmusic_api::song {

// 歌曲下载url返回值
struct SongDownloadUrlResult {
  Req1Type req_1;
};

struct SongFileFormat {
  std::string_view s; // 前缀，如 "C400"
  std::string_view e; // 扩展名，如 ".m4a"
};

constexpr SongFileFormat m4a_format     = {"C40", "m4a"};
constexpr SongFileFormat mp3_128_format = {"M50", "mp3"};
constexpr SongFileFormat mp3_320_format = {"M80", "mp3"};
constexpr SongFileFormat ape_format     = {"A00", "ape"};
constexpr SongFileFormat flac_format    = {"F00", "flac"};
} // namespace qqmusic_api
