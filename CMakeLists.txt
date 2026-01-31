cmake_minimum_required(VERSION 3.22)
project(mudirc
  VERSION 0.1.0
  LANGUAGES C
)

include(cmake/config.cmake)
include(cmake/sanitizers.cmake)

add_subdirectory(sources/preludec)
add_subdirectory(sources/sqlite3)
