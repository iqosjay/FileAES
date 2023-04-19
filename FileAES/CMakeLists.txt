cmake_minimum_required(VERSION 3.10.4)

set(APPNAME "fileaes")
project(${APPNAME})

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_HOME_DIRECTORY})

add_executable(${APPNAME}
        cipher/aes.c
        cipher/file-aes.cpp
        util/logger.cpp
        main.cpp)
