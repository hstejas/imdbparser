cmake_minimum_required(VERSION 3.2)

project(imdbparser)

set(CMAKE_CXX_STANDARD 11)
add_executable(imdbparser ImdbParser.cpp main.cpp ImdbParser.h)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(SQLite3)

if (SQLITE3_FOUND)
   include_directories(${SQLITE3_INCLUDE_DIRS})
   target_link_libraries (imdbparser ${SQLITE3_LIBRARIES})
   add_definitions(-DUSE_SQLITE3)
endif (SQLITE3_FOUND)

