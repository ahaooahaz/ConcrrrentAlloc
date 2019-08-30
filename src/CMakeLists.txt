cmake_minimum_required(VERSION 2.8)
project(ConcurrentMemoryPool)

add_definitions("-std=c++11")
aux_source_directory(${CMAKE_SOURCE_DIR} DIR_SRCS)

add_library(ConcurrentMemoryPool SHARED ${DIR_SRCS})
