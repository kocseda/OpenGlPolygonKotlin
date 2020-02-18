
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fPIC  -shared -Wno-error, -Wno-format-security")
set (common_src Source/esShader.c Source/esShapes.c Source/esTransform.c Source/esUtil.c)

add_library(polygon SHARED rectangle.cpp triangulate.cpp)
find_library(log-lib log )
include_directories(Includes)
target_link_libraries(polygon GLESv3 EGL ${log-lib} )