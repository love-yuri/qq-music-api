cmake_minimum_required(VERSION 3.29)

# windows 使用vcpkg
if (WIN32)
  if (DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
    message(STATUS "VCPKG_ROOT 使用: $ENV{VCPKG_ROOT}")
  else ()
    set(VCPKG_ROOT "E:/love-yuri/github/vcpkg")
    message(WARNING "VCPKG_ROOT 没有设置, 使用默认: ${VCPKG_ROOT}")
  endif ()

  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif ()

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "Suppress developer warnings")
set(CMAKE_CXX_STANDARD 23)

# 实验性 import std 支持
if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3.0")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.2.0" )
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.0")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
endif()

set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(qq_music_api VERSION 1.0 LANGUAGES CXX)

# packages
find_package(CURL REQUIRED)
find_package(glaze CONFIG REQUIRED)

file(GLOB_RECURSE CPP_MODULES
    ${PROJECT_SOURCE_DIR}/src/*.cppm
    ${PROJECT_SOURCE_DIR}/src/core/*.cppm
    ${PROJECT_SOURCE_DIR}/src/playlist/*.cppm
    ${PROJECT_SOURCE_DIR}/src/song/*.cppm
)

# 使用 STATIC 库
add_library(${PROJECT_NAME} STATIC ${CPP_MODULES})

# 仅内部可见，json_utils.hpp 不对外暴露
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PUBLIC
    CURL::libcurl
    glaze::glaze
)

# Modules
target_sources(${PROJECT_NAME}
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    ${CPP_MODULES}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_SCAN_FOR_MODULES ON)

# 编译选项
if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC
      /Zc:preprocessor
      /utf-8
      /wd5050 # 关闭警告: 模块未使用 UTF-8 编码
  )
endif()

# 辅助函数: 将 script/ 复制到指定目标的输出目录
# 消费方应在自己的 CMakeLists.txt 中调用: qq_music_api_copy_scripts(target_name)
function(qq_music_api_copy_scripts TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/script"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/script"
        COMMENT "Copying qq-music-api script/ to ${TARGET_NAME} output directory..."
    )
endfunction()

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)
