#pragma once

#include <string>
#include "glaze/glaze.hpp"
#include "yuri_log.hpp"

template <typename T>
T read_json(std::string_view json) {
  const auto res = glz::read_json<T>(json);
  if (res) {
    return res.value();
  }

  yerror << "json反序列化失败：" << glz::format_error(res, json);
  return T {};
}
