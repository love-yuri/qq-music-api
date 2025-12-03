module;

#include <cstdint>
#include <string>
#include <vector>

export module qqmusic.play_list_result;

struct DataType;
struct DisslistType;

struct DisslistType {
  std::string diss_name;
  std::string diss_cover;
  int song_cnt;
  int listen_num;
  int dirid;
  std::uint64_t tid;
};

struct DataType {
  int totoal;
  std::vector<DisslistType> disslist;
};


struct DirinfoType {
    std::uint64_t id;
    int dirid;
    std::string title;
    std::string picurl;
    int picid;
    std::string desc;
    int listennum;
    int ordernum;
    int songnum;
    int show;
};

struct SingerType {
    int id;
    std::string mid;
    std::string name;
    std::string title;
};

struct AlbumType {
    int id;
    std::string mid;
    std::string name;
    std::string title;
    std::string pmid;
};

struct MvType {
    int id;
    std::string vid;
    int vt;
};

struct KsongType {
    int id;
    std::string mid;
};

struct FileType {
    std::string media_mid;
    int size_try;
    int try_begin;
    int try_end;
    int size_24aac;
    int size_48aac;
    int size_96aac;
    int size_128mp3;
    int size_192ogg;
    int size_192aac;
    int size_320mp3;
    int size_flac;
    int size_ape;
    int size_dts;
    int size_hires;
    int hires_sample;
    int hires_bitdepth;
    int b_30s;
    int e_30s;
    int size_96ogg;
    int size_dolby;
    std::vector<int> size_new;
};

struct VolumeType {
    double gain;
    double peak;
    double lra;
};

struct PayType {
    int pay_month;
    int price_track;
    int price_album;
    int pay_play;
    int pay_down;
    int pay_status;
    int time_free;
};

struct SonglistType {
    int id;
    std::string mid;
    std::string name;
    std::string label;
    std::string title;
    std::string subtitle;
    int interval;
    int language;
    int genre;
    int fnote;
    std::string time_public;
    std::vector<SingerType> singer;
    AlbumType album;
    MvType mv;
    KsongType ksong;
    FileType file;
    VolumeType volume;
    PayType pay;
};

struct DataType1 {
    int from_gedan_plaza;
    DirinfoType dirinfo;
    std::vector<SonglistType> songlist;
};

struct Req1Type {
    int code;
    DataType1 data;
};

export namespace qqmusic_api::playlist {
// 用户歌单详情
struct UserPlaylistsDetailResult {
  int code{};
  Req1Type req_1;
};

// 用户歌单列表
struct UserPlaylistsResult {
  int code;
  std::string message;
  DataType data;
};

}
