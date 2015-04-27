
cmake_minimum_required(VERSION 2.8)
project (compositor)

add_executable(compositor server.cpp compositor.cpp surface.cpp)
add_executable(client client.cpp)

find_package(PkgConfig)

find_package(X11 REQUIRED)

# cmake 3.1 or later
#set(PKG_CONFIG_PATH /usr/local/lib/pkgconfig)
set($ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -g -O0")

pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GLES2 REQUIRED glesv2)

target_link_libraries(compositor
  ${WAYLAND_SERVER_LIBRARIES}
  ${X11_LIBRARIES}
  ${EGL_LIBRARIES}
  ${GLES2_LIBRARIES}
  )

target_link_libraries(client ${WAYLAND_CLIENT_LIBRARIES} ${WAYLAND_EGL_LIBRARIES} ${EGL_LIBRARIES})
