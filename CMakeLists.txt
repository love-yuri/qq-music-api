cmake_minimum_required(VERSION 3.29)

# windows 使用vcpkg
if (WIN32)
  if (DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT "E:/love-yuri/github/vcpkg")
    message("VCPKG_ROOT 使用: $ENV{VCPKG_ROOT}")
  else ()
    set(VCPKG_ROOT "E:/love-yuri/github/vcpkg")
    message(WARNING "VCPKG_ROOT 没有设置, 使用默认: ${VCPKG_ROOT}")
  endif ()

  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif ()

project(qq_music_api VERSION 1.0 LANGUAGES CXX)

# config
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API TRUE)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

# packages
find_package(CURL REQUIRED)
find_package(glaze CONFIG REQUIRED)

file(GLOB IXX_MODULES
    ${PROJECT_SOURCE_DIR}/*.ixx
    ${PROJECT_SOURCE_DIR}/include/*.ixx
    ${PROJECT_SOURCE_DIR}/include/modules/*.ixx
)

file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${IXX_MODULES}
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/utils/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# 使用 STATIC 库
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

# include and link
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC
    CURL::libcurl
    glaze::glaze
)

# Modules
target_sources(${PROJECT_NAME}
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    ${IXX_MODULES}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_SCAN_FOR_MODULES ON)

# 编译选项
if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC /Zc:preprocessor)
endif()

# example_main
add_executable(example_main examples/main.cpp)
target_link_libraries(example_main PRIVATE ${PROJECT_NAME})
