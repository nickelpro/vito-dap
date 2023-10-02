cmake_minimum_required(VERSION 3.24)

if(NOT SKIP_VCPKG)
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    include(FetchContent)
    FetchContent_Declare(
      vcpkg
      GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
      GIT_TAG master
      GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(vcpkg)
    set(CMAKE_TOOLCHAIN_FILE
      ${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake
      CACHE FILEPATH "Vcpkg toolchain file"
    )
    set(VCPKG_ROOT_DIR ${vcpkg_SOURCE_DIR} CACHE PATH "Vcpkg Root Directory")
  endif()

  add_custom_target(UpdateVcpkgBaseline
    ${VCPKG_ROOT_DIR}/vcpkg x-update-baseline
  )
endif()

project(vito-dap VERSION 0.0.9)

find_package(nlohmann_json CONFIG REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(vdap INTERFACE)
target_link_libraries(vdap INTERFACE nlohmann_json::nlohmann_json)
target_sources(vdap INTERFACE FILE_SET HEADERS FILES vdap/dap.hpp)
target_compile_features(vdap INTERFACE cxx_std_20)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vdap.pc.in
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
install(
  EXPORT vdapTarget
  NAMESPACE vdap::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/vdap
)

option(BUILD_TESTS "Builds test binaries and adds them to CTest" FALSE)

if(BUILD_TESTS)
  add_executable(test test/test.cpp)
  target_link_libraries(test PRIVATE vdap)
endif()
