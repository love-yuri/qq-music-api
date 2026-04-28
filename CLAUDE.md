# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

使用 C++23 实现的 QQ 音乐 API 封装库（静态库），依赖 libcurl 进行 HTTP 请求，通过 Node.js 子进程处理签名/加解密。

## 构建命令

```bash
# 配置 (Windows 使用 vcpkg, 需设置 VCPKG_ROOT 或使用默认路径 E:/love-yuri/github/vcpkg)
mkdir -p build && cd build
cmake .. -G Ninja
cmake --build .
```

- CMake >= 3.29 + Ninja
- C++23，支持 modules（MSVC / GCC >= 15 / Clang >= 19）
- 依赖：libcurl、[glaze](https://github.com/stephenberry/glaze)（JSON 序列化）、Node.js（运行时加解密）
- MSVC 需 `/Zc:preprocessor /utf-8`

## 架构

```
src/
├── qq_music_api.cppm    # 聚合模块，re-export 所有子模块
├── log.cppm             # 日志库（yinfo/yerror/ywarn/ydebug 宏）
├── json_utils.hpp       # JSON 反序列化工具（内部使用，不公开）
├── curl.cppm            # HTTP 客户端封装（GET/POST/POST_JSON/POST_FORM）
├── config.cppm          # 全局配置单例（qq号、cookie、登录状态）
├── nodejs.cppm          # Node.js 子进程桥接（sign/encrypt/decrypt）
└── api.cppm             # 所有 API 函数及返回值结构体（歌单+歌曲）
script/
└── crypto_util.js       # QQ 音乐签名/加密/解密 Node.js 脚本
json-format/             # Vue 3 辅助工具（独立 npm 项目）
```

### 模块系统

所有文件均为 C++23 模块（`.cppm`），统一使用 `qq_music_api` 作为模块名：

- **聚合模块**：`qq_music_api` — re-export 所有子模块
- **子模块（partition）**：`qq_music_api:curl`、`qq_music_api:config`、`qq_music_api:nodejs`、`qq_music_api:api`
- **独立命名模块**：`qq_music_api.log` — 日志库
- **使用方式**：`import qq_music_api;` 即可导入全部功能

### 请求流程

大部分 API（除了简单的 GET）遵循此流程：
1. 构造包含 `comm` 和 `req_1` 的 JSON 数据
2. `nodejs::get_sign()` → 获取签名拼接到 URL
3. `nodejs::get_encrypt()` → 加密数据作为 POST body
4. `curl::post()` 发送请求
5. `nodejs::get_decrypt()` → 解密响应
6. `read_json<T>()` → glaze 反序列化为 C++ 结构体

### 关键设计

- **错误处理**：HTTP 层使用 `std::expected<std::string, CurlError>`，API 层直接抛异常或返回空值
- **模块系统**：所有 `.cppm` 为 `qq_music_api` 模块的 partition，通过 `FILE_SET cxx_modules` 注册
- **命名空间**：API 在 `qqmusic_api::playlist` / `qqmusic_api::song` 下，HTTP 在 `curl` namespace 下
- **无 #include**：除 global module fragment 中必要的 C 头文件（`<curl/curl.h>`、`<windows.h>`、`<ctime>`）外，统一使用 `import std;` 和 `import :partition;`
- **内部头文件**：`json_utils.hpp` 仅在 `api.cppm` 的 global module fragment 中 include，不对外暴露
- **CMake 辅助函数**：`qq_music_api_copy_scripts(target)` 用于将 `script/` 拷贝到消费方可执行文件目录

## 编码规范

- 语言：中文注释、中文 commit message
- 格式化：LLVM 风格，2 空格缩进，无列宽限制（`ColumnLimit: 0`），`BinPackParameters: false`
- 命名：snake_case（函数/变量），PascalCase（类/结构体），与现有代码保持一致
- 日志：使用 `yuri::info` / `yuri::error` / `yuri::warn` / `yuri::debug` 函数
- 模块导入：`import qq_music_api;` 或 `import :partition_name;`

## json-format 工具

```bash
cd json-format && npm install
npm run dev    # 开发模式
npm run build  # 构建
```

Vue 3 + Vite + Tailwind，用于将 API 返回的 JSON 转为对应的 C++ struct 定义。
