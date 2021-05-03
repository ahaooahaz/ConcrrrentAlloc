CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(ConcurrentMemoryPool_unitest)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g -pthread")

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/../src SRC)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../src)

# ADD_EXECUTABLE(TEST_VECTOR ${PROJECT_SOURCE_DIR}/../src/ConcurrentAlloc.hpp
#                         ${PROJECT_SOURCE_DIR}/test_vector.cpp)

ADD_EXECUTABLE(TEST_PAGECACHE ${PROJECT_SOURCE_DIR}/../src/PageCache.cpp
                                ${PROJECT_SOURCE_DIR}/test_pagecache.cpp)

ADD_EXECUTABLE(TEST_CENTRALCACHE ${PROJECT_SOURCE_DIR}/../src/CentralCache.cpp
                                ${PROJECT_SOURCE_DIR}/../src/PageCache.cpp
                                ${PROJECT_SOURCE_DIR}/test_centralcache.cpp)
