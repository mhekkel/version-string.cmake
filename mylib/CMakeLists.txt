cmake_minimum_required(VERSION 3.15)
project(mylib VERSION 1.0.0 LANGUAGES CXX)

add_library(mylib ${PROJECT_SOURCE_DIR}/src/mylib.cpp ${PROJECT_SOURCE_DIR}/src/mylib-2.cpp)
add_library(mylib::mylib ALIAS mylib)

add_version_header(mylib "${CMAKE_CURRENT_SOURCE_DIR}/src/revision.hpp" LIB_NAME "mylib")

target_include_directories(mylib
	PUBLIC
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

