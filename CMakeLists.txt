cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file"
)

project(vito-dap CXX)

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(dap dap.cpp dap.hpp)
target_compile_features(dap PRIVATE cxx_std_20)
target_compile_options(dap PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(dap PRIVATE fmt::fmt-header-only nlohmann_json::nlohmann_json)
