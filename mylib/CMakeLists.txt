cmake_minimum_required(VERSION 3.15)
project(mylib VERSION 1.0.0 LANGUAGES CXX)

write_version_header("${PROJECT_SOURCE_DIR}/src" LIB_NAME "mylib")

add_library(mylib ${PROJECT_SOURCE_DIR}/src/mylib.cpp ${PROJECT_SOURCE_DIR}/src/mylib-2.cpp)
add_library(mylib::mylib ALIAS mylib)

target_include_directories(mylib
	PUBLIC
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

