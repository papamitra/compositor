
cmake_minimum_required(VERSION 2.8)
project (compositor)

add_executable(compositor compositor.cpp)

find_package(PkgConfig)

# cmake 3.1 or later
#set(PKG_CONFIG_PATH /usr/local/lib/pkgconfig)

set($ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")

pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server)

target_link_libraries(compositor ${WAYLAND_SERVER_LIBRARIES})
