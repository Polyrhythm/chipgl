cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++14 -fPIC")
project (ChipGL)

add_executable(chipgl src/chipgl.cpp)

# ------------
# Dependencies
# ------------
include_directories(${PROJECT_SOURCE_DIR}/src)

# add X11
find_package(X11 REQUIRED)
message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

# add eglo lib
add_library(eglo src/eglo.cpp)
add_library(filewatcher src/filewatcher.cpp)

INCLUDE(FindPkgConfig)
#GLM
INCLUDE_DIRECTORIES(
  deps/glm-0.9.8.4
)

target_link_libraries(chipgl
  ${X11_LIBRARIES}
  EGL
  GLESv2
  GLESv1_CM
  eglo
  filewatcher
)
