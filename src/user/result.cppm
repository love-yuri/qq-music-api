/*
 * @Description: 用户 API 返回值结构体
 */
module;
export module qq_music_api:user_result;

import std;

export namespace qqmusic_api::user {

struct UserUrlBykeyType {
  std::string url_key;    // 跳转 key
  std::string url_params; // 跳转参数
};

struct UserSingerInfoType {
  int singerid{}; // 歌手 id
};

struct UserProfileTypeInfoType {
  int type{};                         // 类型
  std::string jumpurl;                // 跳转链接
  UserUrlBykeyType cfinfo_bykey;      // 跳转信息
  std::string jumpkey;                // 跳转 key
  std::string iconurl;                // 图标链接
};

struct UserLvInfoType {
  std::string iconurl;           // 等级图标
  std::string jumpurl;           // 跳转链接
  std::string jumpkey;           // 跳转 key
  UserUrlBykeyType lvinfo_bykey; // 跳转信息
};

struct UserInfoUIColorType {
  std::string lightColor; // 浅色主题颜色
  std::string darkColor;  // 深色主题颜色
};

struct UserInfoUIIconType {
  int width{};        // 图标宽度
  int height{};       // 图标高度
  std::string srcUrl; // 图标链接
  std::string style;  // 图标样式
  std::string ext;    // 扩展跳转链接
  std::string desc;   // 扩展描述
};

struct UserInfoUIType {
  UserInfoUIColorType nickname;           // 昵称样式
  std::vector<UserInfoUIIconType> iconlist; // 图标列表
};

struct UserMedalType {
  int flag{};                         // 勋章标记
  std::string iconurl;                // 图标链接
  std::string jumpurl;                // 跳转链接
  std::string jumpkey;                // 跳转 key
  UserUrlBykeyType medal_bykey;       // 跳转信息
};

struct UserListenInfoType {
  std::string iconurl;               // 听歌等级图标
  std::string jumpurl;               // 跳转链接
  std::string jumpkey;               // 跳转 key
  UserUrlBykeyType listen_bykey;     // 跳转信息
};

struct UserBackpicType {
  std::string picurl; // 背景图
  int type{};         // 背景图类型
  std::string title;  // 标题
};

struct UserCfInfoType {
  std::string title;              // 标题
  std::string jumpurl;            // 跳转链接
  std::string jumpkey;            // 跳转 key
  UserUrlBykeyType cfinfo_bykey;  // 跳转信息
  int similar{};                  // 相似度
};

struct UserNumsType {
  int visitornum{};      // 访客数
  int fansnum{};         // 粉丝数
  int follownum{};       // 关注数
  int followusernum{};   // 关注用户数
  int followsingernum{}; // 关注歌手数
  int frdnum{};          // 好友数
};

struct UserCreatorType {
  std::string nick;                   // 昵称
  std::string headpic;                // 头像
  std::string ifpic;                  // 图片
  std::uint64_t uin{};                // QQ 号
  int forbidden{};                    // 禁用状态
  int ishost{};                       // 是否本人
  int is_bind_weibo{};                // 是否绑定微博
  std::string weibo_uid;              // 微博 uid
  std::string weibo_nick;             // 微博昵称
  std::string extra;                  // 扩展信息
  UserSingerInfoType singerinfo;      // 歌手信息
  std::string uin_web;                // Web uin
  std::string encrypt_uin;            // 加密 uin
  int isfollow{};                     // 是否关注
  int islock{};                       // 是否锁定
  int buy_lock{};                     // 购买锁定
  int fav_lock{};                     // 收藏锁定
  int diss_lock{};                    // 歌单锁定
  std::string shareurl;               // 分享链接
  UserUrlBykeyType share_bykey;       // 分享跳转信息
  std::string jumpkey;                // 跳转 key
  UserProfileTypeInfoType typeinfo;   // 类型信息
  std::vector<UserLvInfoType> lvinfo; // 等级信息
  UserInfoUIType userInfoUI;          // UI 信息
  UserMedalType medal;                // 勋章信息
  UserListenInfoType listeninfo;      // 听歌信息
  UserBackpicType backpic;            // 背景图
  UserCfInfoType cfinfo;              // 主页信息
  UserNumsType nums;                  // 统计信息
};

struct UserMusicItemType {
  std::string title;             // 标题
  std::string picurl;            // 图片链接
  std::string laypic;            // 覆盖层图片
  std::string subtitle;          // 副标题
  std::string jumpurl;           // 跳转链接
  int jumptype{};                // 跳转类型
  std::string jumpkey;           // 跳转 key
  std::string id;                // 业务 id
  UserUrlBykeyType music_bykey;  // 跳转信息
  int type{};                    // 类型
  int num0{};                    // 数量 0
  int num1{};                    // 数量 1
  int num2{};                    // 数量 2
};

struct UserDissItemType {
  std::uint64_t dissid{}; // 歌单 id
  int dirid{};            // 目录 id
  std::string picurl;     // 封面
  std::string title;      // 标题
  std::string subtitle;   // 副标题
  int icontype{};         // 图标类型
  std::string iconurl;    // 图标链接
  int isshow{};           // 是否显示
  int dir_show{};         // 目录是否显示
};

struct UserDissType {
  int num{};                         // 歌单数量
  std::string title;                 // 标题
  std::string laypic;                // 覆盖层图片
  std::string jumpurl;               // 跳转链接
  std::vector<UserDissItemType> list; // 歌单列表
};

struct UserEmptyListItemType {
};

struct UserVideoType {
  int num{};                                // 视频数量
  std::string title;                        // 标题
  std::string jumpurl;                      // 跳转链接
  std::string jumpkey;                      // 跳转 key
  std::vector<UserEmptyListItemType> list;  // 视频列表
};

struct UserArticleType {
  std::string title;                        // 标题
  std::string jumpurl;                      // 跳转链接
  std::string jumpkey;                      // 跳转 key
  std::string laypic;                       // 覆盖层图片
  int totalcnt{};                           // 总数
  std::vector<UserEmptyListItemType> list;  // 文章列表
};

struct UserRadioType {
  std::string title;                        // 标题
  std::string jumpurl;                      // 跳转链接
  std::string jumpkey;                      // 跳转 key
  std::string laypic;                       // 覆盖层图片
  int totalcnt{};                           // 总数
  std::vector<UserEmptyListItemType> list;  // 电台列表
};

struct UserInfoDataType {
  UserCreatorType creator;             // 创建者信息
  std::string mymusictype;             // 我的音乐标题
  std::vector<UserMusicItemType> mymusic; // 我的音乐
  UserDissType mydiss;                 // 我的歌单
  UserVideoType video;                 // 视频
  UserArticleType myarticle;           // 专栏文章
  UserRadioType myradio;               // 主播电台
};

struct UserInfoResult {
  int code{};             // 返回码
  int subcode{};          // 子返回码
  std::string msg;        // 消息
  std::string url;        // 失败跳转链接
  UserInfoDataType data;  // 用户信息
};

} // namespace qqmusic_api::user
