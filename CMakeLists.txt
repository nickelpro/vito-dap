cmake_minimum_required(VERSION 3.23)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file"
)

project(vito-dap LANGUAGES CXX VERSION 0.0.5)

find_package(nlohmann_json CONFIG REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(vdap INTERFACE)
target_link_libraries(vdap INTERFACE nlohmann_json::nlohmann_json)
target_sources(vdap INTERFACE FILE_SET HEADERS FILES vdap/dap.hpp)
target_compile_features(vdap INTERFACE cxx_std_20)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg-config.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/vdap.pc
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/vdap.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/vdap-version.cmake
  COMPATIBILITY AnyNewerVersion
  ARCH_INDEPENDENT
)
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vdap-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/vdap-version.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/vdap
)

install(
  TARGETS vdap
  EXPORT vdapTarget
  FILE_SET HEADERS
)
install(EXPORT vdapTarget NAMESPACE vdap:: DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/vdap)

option(BUILD_TESTS "Builds test binaries and adds them to CTest" FALSE)
if(BUILD_TESTS)
  add_executable(test test/test.cpp)
  target_link_libraries(test PRIVATE vdap)
endif()
