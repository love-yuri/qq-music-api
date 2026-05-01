/*
 * @Description: 共享内部类型（歌单 + 歌曲域的 JSON 反序列化中间结构体）
 */
module;
export module qq_music_api:detail;

import std;

export {

struct DisslistType {
  std::string diss_name;   // 歌单名称
  std::string diss_cover;  // 歌单封面
  int song_cnt;            // 歌曲数量
  int listen_num;          // 听众数
  int dirid;               // 目录id
  std::uint64_t tid;       // 歌单tid
};

struct PlaylistDataType {
  int totoal;                         // 总数
  std::vector<DisslistType> disslist; // 歌单列表
};

struct DirinfoType {
  std::uint64_t id;    // 歌单id
  int dirid;           // 目录id
  std::string title;   // 标题
  std::string picurl;  // 封面url
  int picid;           // 封面id
  std::string desc;    // 描述
  int listennum;       // 听众数
  int ordernum;        // 排序号
  int songnum;         // 歌曲数量
  int show;            // 是否显示
};

// ======================== 共享类型（歌手、专辑、文件等） ========================
struct SingerType {
  int id;             // 歌手id
  std::string mid;    // 歌手mid
  std::string name;   // 歌手名
  std::string title;  // 歌手标题
};

struct AlbumType {
  int id;             // 专辑id
  std::string mid;    // 专辑mid
  std::string name;   // 专辑名
  std::string title;  // 专辑标题
  std::string pmid;   // 专辑pmid
};

struct MvType {
  int id;             // mv id
  std::string vid;    // mv vid
  int vt;             // mv类型
};

struct KsongType {
  int id;             // k歌id
  std::string mid;    // k歌mid
};

struct FileType {
  std::string media_mid; // 媒体mid
  int size_try;          // 试听大小
  int try_begin;         // 试听开始
  int try_end;           // 试听结束
  int size_24aac;        // 24k aac大小
  int size_48aac;        // 48k aac大小
  int size_96aac;        // 96k aac大小
  int size_128mp3;       // 128k mp3大小
  int size_192ogg;       // 192k ogg大小
  int size_192aac;       // 192k aac大小
  int size_320mp3;       // 320k mp3大小
  int size_flac;         // flac大小
  int size_ape;          // ape大小
  int size_dts;          // dts大小
  int size_hires;        // hires大小
  int hires_sample;      // hires采样率
  int hires_bitdepth;    // hires位深
  int b_30s;             // 30秒开始
  int e_30s;             // 30秒结束
  int size_96ogg;        // 96k ogg大小
  int size_dolby;        // 杜比大小
  std::vector<int> size_new; // 新格式大小
};

struct VolumeType {
  double gain; // 增益
  double peak; // 峰值
  double lra;  // 响度范围
};

struct PayType {
  int pay_month;    // 包月
  int price_track;  // 单曲价格
  int price_album;  // 专辑价格
  int pay_play;     // 播放付费
  int pay_down;     // 下载付费
  int pay_status;   // 付费状态
  int time_free;    // 免费时间
};

struct SonglistType {
  int id;                         // 歌曲id
  std::string mid;                // 歌曲mid
  std::string name;               // 歌曲名
  std::string label;              // 标签
  std::string title;              // 标题
  std::string subtitle;           // 子标题
  int interval;                   // 时长
  int language;                   // 语言
  int genre;                      // 流派
  int fnote;                      // 音符
  std::string time_public;        // 发布时间
  std::vector<SingerType> singer; // 歌手列表
  AlbumType album;                // 专辑
  MvType mv;                      // MV
  KsongType ksong;                // K歌
  FileType file;                  // 文件信息
  VolumeType volume;              // 音量信息
  PayType pay;                    // 付费信息
};

// ======================== 歌单域（依赖共享类型） ========================

struct PlaylistDataType1 {
  int from_gedan_plaza;               // 来自歌单广场
  DirinfoType dirinfo;                // 歌单信息
  std::vector<SonglistType> songlist; // 歌曲列表
};

struct PlaylistReq1Type {
  int code;               // 返回码
  PlaylistDataType1 data; // 数据
};

// ======================== 歌曲域内部结构体 ========================

struct MidurlinfoType {
  std::string filename; // 文件名
  std::string purl;     // 播放url
};

struct SongDataType {
  std::vector<std::string> sip;          // 服务器ip列表
  std::vector<MidurlinfoType> midurlinfo; // url信息列表
};

struct SongReq1Type {
  int code{};          // 返回码
  SongDataType data;   // 数据
};

}