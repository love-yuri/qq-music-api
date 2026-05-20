/*
 * @Description: 歌曲 API 内部实现（原始 HTTP 请求，不含 JSON 反序列化）
 */
module;
#include "core/json_utils.hpp"
module qq_music_api:song_detail;

import std;
import :log;
import :curl;
import :config;
import :nodejs;
import :song_result;

namespace qqmusic_api::song::detail {

/**
 * 获取歌曲的下载链接
 * @param mid 歌曲mid
 * @param format 目标格式，请确保歌曲有该音源
 * @return 原始响应字符串
 */
std::string get_song_download_url(const std::string_view mid, const SongFileFormat &format = m4a_format) {
  if (qqmusic_api_config.qq.empty() || qqmusic_api_config.cookie.empty()) {
    throw std::runtime_error(std::format("{} 需要用户qq以及cookie!", __FUNCTION__));
  }

  constexpr std::string_view base_url = "https://u6.y.qq.com/cgi-bin/musics.fcg?_=1764590219737&encoding=ag-1&sign=";
  constexpr auto sign_data_json = R"(
    {{
      "comm": {{
        "cv": 4747474,
        "ct": 24,
        "format": "json",
        "inCharset": "utf-8",
        "outCharset": "utf-8",
        "notice": 0,
        "platform": "yqq.json",
        "needNewCode": 1,
        "g_tk_new_20200303": 2050245758,
        "g_tk": 2050245758
      }},
      "req_1": {{
        "module": "music.vkey.GetEVkey",
        "method": "GetUrl",
        "param": {{
          "filename": ["{0}"],
          "guid": "5737980864",
          "songmid": [
              "{1}"
          ],
          "songtype": [1],
          "loginflag": 1,
          "platform": "20",
          "xcdn": 1
        }}
      }}
    }}
  )";

  const auto filename = std::format("{}{}.{}", format.s, mid, format.e);
  const auto sign_json = std::format(sign_data_json, filename, mid);
  const auto url = std::string(base_url) + nodejs::get_sign(sign_json);
  const auto sign_data = nodejs::get_encrypt(sign_json);
  const curl::KeyValueList headers = {
    {"cookie", qqmusic_api_config.cookie},
  };

  return nodejs::get_decrypt(curl::post(url, sign_data, headers).value());
}

/**
 * 获取歌曲可直接下载的完整 URL
 * @param mid 歌曲 mid
 * @param format 目标文件格式
 * @return 完整下载 URL，失败时返回空字符串
 */
std::string resolve_song_download_url(const std::string_view mid, const SongFileFormat &format = m4a_format) {
  const auto res = get_song_download_url(mid, format);
  if (const auto [sip, midurlinfo] = read_json<SongDownloadUrlResult>(res).req_1.data;
      !sip.empty() && !midurlinfo.empty()) {
    return sip.front() + midurlinfo.front().purl;
  }

  qqmusic_api::error("无法找到url，请检查format格式是否正确!");
  return {};
}

/**
 * 清理文件名中的非法字符
 * @param name 原始文件名
 * @return 可写入当前文件系统的文件名
 */
std::string sanitize_file_name(std::string name) {
  constexpr std::string_view invalid = R"(<>:"/\|?*)";
  for (char &ch : name) {
    if (static_cast<unsigned char>(ch) < 32 || invalid.find(ch) != std::string_view::npos) {
      ch = '_';
    }
  }

  while (!name.empty() && (name.back() == '.' || name.back() == ' ')) {
    name.pop_back();
  }

  return name.empty() ? "unknown" : std::move(name);
}

/**
 * 生成歌曲缓存文件名主体
 * @param song 歌曲下载信息
 * @return 不包含扩展名的安全文件名
 */
std::string make_file_stem(const SongDownloadInfo &song) {
  auto file_stem = song.title;
  if (!song.artist.empty()) {
    file_stem += " - ";
    file_stem += song.artist;
  }
  return sanitize_file_name(std::move(file_stem));
}

/**
 * 按播放优先级生成候选音源格式
 * @param song 歌曲下载信息
 * @return 优先级从高到低的格式列表
 */
std::vector<SongFileFormat> preferred_formats(const SongDownloadInfo &song) {
  std::vector<SongFileFormat> formats;
  formats.reserve(5);

  if (song.has_flac) {
    formats.push_back(flac_format);
  }
  if (song.has_mp3_320) {
    formats.push_back(mp3_320_format);
  }
  if (song.has_mp3_128) {
    formats.push_back(mp3_128_format);
  }
  formats.push_back(m4a_format);
  if (song.has_ape) {
    formats.push_back(ape_format);
  }

  return formats;
}

/**
 * 查找歌曲本地缓存文件
 * @param song 歌曲下载信息
 * @param cache_dir 缓存目录
 * @return 已存在的缓存文件路径，未命中时返回空路径
 */
std::filesystem::path cached_song_path(
  const SongDownloadInfo &song,
  const std::filesystem::path &cache_dir = "musics"
) {
  const auto safe_file_stem = make_file_stem(song);
  std::error_code ec;

  for (const auto &[s, e] : preferred_formats(song)) {
    const auto path = cache_dir / std::format("{}.{}", safe_file_stem, e);
    if (std::filesystem::exists(path, ec) && !ec) {
      return path;
    }
  }

  return {};
}

/**
 * 下载歌曲文件到本地缓存
 * 优先使用已有缓存；未命中时按 preferred_formats 顺序尝试下载。
 * @param song 歌曲下载信息
 * @param cache_dir 缓存目录
 * @return 可播放的本地文件路径，失败时返回空路径
 */
std::filesystem::path download_song_file(
  const SongDownloadInfo &song,
  const std::filesystem::path &cache_dir = "musics"
) {
  if (song.mid.empty()) {
    warn("歌曲 mid 为空，无法下载");
    return {};
  }

  if (const auto cached = cached_song_path(song, cache_dir); !cached.empty()) {
    return cached;
  }

  std::error_code ec;
  std::filesystem::create_directories(cache_dir, ec);
  if (ec) {
    error("创建音乐目录失败: {}", ec.message());
    return {};
  }

  const auto safe_file_stem = make_file_stem(song);
  for (const auto &format : preferred_formats(song)) {
    const auto path = cache_dir / std::format("{}.{}", safe_file_stem, format.e);
    const auto url = resolve_song_download_url(song.mid, format);
    if (url.empty()) {
      continue;
    }

    const curl::KeyValueList headers = {
      { "referer", "https://y.qq.com/" },
      { "cookie", qqmusic_api_config.cookie },
      { "user-agent",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/142.0.0.0 Safari/537.36" },
    };
    const auto data = curl::get(url, headers);
    if (!data) {
      error("下载歌曲失败: {}.{}", song.title, format.e);
      continue;
    }

    std::ofstream output(path, std::ios::binary);
    if (!output.is_open()) {
      error("打开音乐文件失败: {}", path.string());
      return {};
    }

    output.write(data->data(), static_cast<std::streamsize>(data->size()));
    info("歌曲下载完成: {}", path.string());
    return path;
  }

  error("获取歌曲下载链接失败: {}", song.title);
  return {};
}

} // namespace qqmusic_api::song::detail
