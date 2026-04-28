#pragma once

#include "glaze/glaze.hpp"
import std;
import qq_music_api.log;

/**
 * 解析选项
 */
constexpr glz::opts options = {
  .comments = true,               // 允许包含注释
  .error_on_unknown_keys = false, // 忽略未知键
  .error_on_missing_keys = false, // 忽略缺少key
};

/**
 * 带选项的 JSON 读取
 */
template <glz::read_supported<glz::JSON> T, glz::is_buffer Buffer>
[[nodiscard]] glz::expected<T, glz::error_ctx> read_with_options(Buffer &&buffer) {
  T value{};
  glz::context ctx{};
  if (const glz::error_ctx ec = read<options>(value, std::forward<Buffer>(buffer), ctx)) {
    return glz::unexpected(ec);
  }
  return value;
}

/**
 * 便捷 JSON 反序列化，失败时日志输出并返回默认值
 */
template <typename T>
T read_json(const std::string_view json) {
  const auto res = read_with_options<T>(json);
  if (res) {
    return res.value();
  }

  yuri::error("json反序列化失败：{}", glz::format_error(res, json));
  return T{};
}
