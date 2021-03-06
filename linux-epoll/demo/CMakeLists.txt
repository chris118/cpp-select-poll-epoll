cmake_minimum_required(VERSION 3.2)
project( demo )
 
# 设置用debug还是release模式。debug允许断点，而release更快
#set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_BUILD_TYPE Release )
 
# 设置编译选项
# 允许c++11标准、O3优化、多线程。match选项可避免一些cpu上的问题
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )
 
# 常见依赖库：cv
#find_package( OpenCV REQUIRED )

# git submodule add <submodule_url>  # 添加子项目
add_subdirectory(glog)

option(BUILD_SERVER_SHARED_LIBRARY "Build server Shared Library" OFF)
option(BUILD_SERVER_STATIC_LIBRARY "Build server Shared Library" ON)
option(BUILD_SERVER_EXAMPLES "Build server Examples" OFF)

list(APPEND SERVER_SRCS
    ${PROJECT_SOURCE_DIR}/src/main.cc)

# 二进制文件输出到bin
#set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )

# 库输出到lib
#set( LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib )
 
# 库文件目录
link_directories(
    ${PROJECT_SOURCE_DIR}/build/glog
    /usr/local/lib
)

# 头文件目录
include_directories (
    ${PROJECT_SOURCE_DIR}/build/glog/glog
    /usr/local/include
)

# #server Static library
# if(BUILD_SERVER_STATIC_LIBRARY)
#     add_executable(isserver STATIC ${SERVER_SRCS})
# endif(BUILD_SERVER_STATIC_LIBRARY)

# #server Static library
# if(BUILD_SERVER_SHARED_LIBRARY)
#     add_executable(isserver SHARED ${SERVER_SRCS})
# endif(BUILD_SERVER_SHARED_LIBRARY)


add_executable(demo ${SERVER_SRCS})
target_link_libraries(demo glog avformat avcodec avutil)