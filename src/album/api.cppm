/*
 * @Description: 专辑 API 内部实现（原始 HTTP 请求）
 */
module;
module qq_music_api:album_detail;

import std;

namespace qqmusic_api::album::detail {

/**
 * 获取专辑封面 URL
 * @param size 图片尺寸
 * @param album_mid 专辑 mid
 * @return 专辑封面 URL
 */
std::string get_album_cover_url(int size, const std::string_view album_mid) {
  size = std::min(size, 800);
  return std::format("https://y.gtimg.cn/music/photo_new/T002R{0}x{0}M000{1}.jpg", size, album_mid);
}

/**
 * 获取专辑封面图片链接
 * @param size 图片尺寸
 * @param album_mid 专辑 mid
 * @return 图片链接
 */
std::string get_album_cover(const int size, const std::string_view album_mid) {
  return get_album_cover_url(size, album_mid);
}

} // namespace qqmusic_api::album::detail
