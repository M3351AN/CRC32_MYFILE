cmake_minimum_required(VERSION 3.16)
project(CRC32_MYFILE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CRC32_MYFILE crc32_myfile.cc miniz.c)
