cmake_minimum_required(VERSION 3.10)
project(MyFileSystem)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES "*.cpp")
add_executable(myfs_app ${SOURCES})
