cmake_minimum_required(VERSION 3.15)

project(version_string VERSION 1.2.0)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(VersionString)

write_version_header("${PROJECT_SOURCE_DIR}/src")

add_subdirectory(mylib)

add_executable(unit-test-1 ${PROJECT_SOURCE_DIR}/src/unit-test.cpp ${PROJECT_SOURCE_DIR}/src/unit-test-extra.cpp)
target_link_libraries(unit-test-1 mylib::mylib)

if(WIN32)
	install(FILES cmake/VersionString.cmake DESTINATION VersionString/cmake RENAME VersionStringConfig.cmake)
	install(FILES cmake/revision.hpp.in DESTINATION VersionString/cmake)
else()
	install(FILES cmake/VersionString.cmake DESTINATION share/VersionString/cmake RENAME VersionStringConfig.cmake)
	install(FILES cmake/revision.hpp.in DESTINATION share/VersionString/cmake)
endif()
