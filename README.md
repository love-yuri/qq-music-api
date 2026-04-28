# qq-music-api

使用 C++23 和 curl 实现的 QQ 音乐 API 封装库（静态库），依赖 libcurl 进行 HTTP 请求，通过 Node.js 子进程处理签名/加解密。

## 快速开始

### 构建依赖

- CMake >= 3.29 + Ninja
- C++23 编译器（MSVC / GCC >= 15 / Clang >= 19）
- libcurl、[glaze](https://github.com/stephenberry/glaze)
- Node.js（运行时加解密）

### 构建

```bash
mkdir -p build && cd build
cmake .. -G Ninja
cmake --build .
```

### CMake 集成

```cmake
add_subdirectory(libs/qq-music-api)
target_link_libraries(${PROJECT_NAME} qq_music_api)
# 将加解密脚本拷贝到可执行文件目录（必须调用）
qq_music_api_copy_scripts(${PROJECT_NAME})
```

## 使用方式

```cpp
import qq_music_api;

int main() {
  // 配置用户信息
  qqmusic_api_config.qq = "你的QQ号";
  qqmusic_api_config.loginFromFile();  // 从 cookie.txt 读取 cookie

  // 获取用户歌单列表
  auto list = qqmusic_api::playlist::get_user_playlists();
  for (auto &pl : list.data.disslist) {
    yuri::info("歌单: {} (tid: {})", pl.diss_name, pl.tid);
  }

  // 获取歌单详情
  auto detail = qqmusic_api::playlist::get_user_playlists_detail(tid, 0, 20);
  for (auto &song : detail.req_1.data.songlist) {
    yuri::info("歌曲: {} - {}", song.name, song.singer[0].name);
  }

  // 获取歌曲下载链接
  std::string url = qqmusic_api::song::get_song_download_url("歌曲mid");
  // 指定格式
  url = qqmusic_api::song::get_song_download_url("歌曲mid", qqmusic_api::song::flac_format);
}
```

## API 列表

### 命名空间 `qqmusic_api::playlist`

| 函数 | 说明 |
|------|------|
| `get_user_playlists(size = 11)` | 获取用户收藏的歌单列表 |
| `get_user_playlists_detail(tid, begin = 0, size = 20)` | 获取歌单详情（含歌曲列表） |
| `add_song_to_playlist(dir_id, song_id)` | 将歌曲添加到歌单（需 QQ + cookie） |
| `delete_song_from_playlist(dir_id, song_id)` | 从歌单移除歌曲（需 QQ + cookie） |

### 命名空间 `qqmusic_api::song`

| 函数 | 说明 |
|------|------|
| `get_song_download_url(mid, format = m4a_format)` | 获取歌曲下载链接（需 QQ + cookie） |

### 支持的音频格式

| 常量 | 格式 |
|------|------|
| `m4a_format` | M4A（默认） |
| `mp3_128_format` | MP3 128kbps |
| `mp3_320_format` | MP3 320kbps |
| `ape_format` | APE 无损 |
| `flac_format` | FLAC 无损 |

### 全局配置

| 变量 | 说明 |
|------|------|
| `qqmusic_api_config.qq` | 用户 QQ 号 |
| `qqmusic_api_config.cookie` | 用户 cookie |
| `qqmusic_api_config.has_login` | 登录状态 |
| `qqmusic_api_config.loginFromFile(path = "cookie.txt")` | 从文件读取 cookie |
