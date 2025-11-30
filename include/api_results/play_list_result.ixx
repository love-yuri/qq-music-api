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

#include <string>
#include <vector>

struct Req1Type;
struct DataType;
struct DirinfoType;
struct SonglistType;
struct SingerType;
struct AlbumType;
struct MvType;
struct KsongType;
struct FileType;
struct VolumeType;
struct PayType;
struct CmtURLBykeyType;

struct DirinfoType {
    int id;
    int dirid;
    std::string title;
    std::string picurl;
    std::string desc;
    int listennum;
    int ordernum;
    int dirtype;
    int songnum;
    int show;
    int owndir;
    std::string headurl;
    std::vector<void*> tag;
    int role;
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
    std::vector<void*> size_360ra;
    int size_dolby;
    std::vector<int> size_new;
};

struct VolumeType {
    double gain;
    int peak;
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
    int type;
    int songtype;
    std::string mid;
    std::string name;
    std::string title;
    std::string subtitle;
    int language;
    int genre;
    int fnote;
    std::string url;
    std::string time_public;
    std::vector<SingerType> singer;
    AlbumType album;
    MvType mv;
    KsongType ksong;
    FileType file;
    VolumeType volume;
    PayType pay;
    int tid;
    int bpm;
    std::string ktag;
    std::string team;
    std::vector<double> vf;
    int bf;
};

struct CmtURLBykeyType {
    std::string url_key;
    std::string url_params;
};

struct Req1Type {
    DataType data;
};

// 歌单详情
export struct UserPlaylistsDetailResult {
    int code{};
    Req1Type req_1;
};

// 用户歌单列表
export struct UserPlaylistsResult {
  int code;
  std::string message;
  DataType data;
};
