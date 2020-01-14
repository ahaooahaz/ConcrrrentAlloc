cmake_minimum_required(VERSION 2.8)
project(ConcurrentMemoryPool)

add_definitions("-std=c++11")
aux_source_directory(./src DIR_SRCS)

include_directories(${PROJECT_SOURCE_DIR}/third_party/glog/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/gflags/include)

link_directories(${PROJECT_SOURCE_DIR}/third_party/glog/lib)
link_directories(${PROJECT_SOURCE_DIR}/third_party/gflags/lib)

add_library(ConcurrentMemoryPool SHARED ${DIR_SRCS})
target_link_libraries(ConcurrentMemoryPool libglog.a libgflags.a -lpthread)
