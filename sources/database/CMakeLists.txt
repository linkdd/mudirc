project(database C)

## MARK: library
file(GLOB_RECURSE DATABASE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(${PROJECT_NAME} STATIC ${DATABASE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# MARK: tool sqlgen
file(GLOB_RECURSE DATABASE_SQLGEN_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/sqlgen/*.c
)

add_executable(${PROJECT_NAME}-sqlgen ${DATABASE_SQLGEN_SOURCES})

target_compile_options(${PROJECT_NAME}-sqlgen PRIVATE
  -Wall -Wextra -Werror
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_definitions(${PROJECT_NAME}-sqlgen PRIVATE DEBUG)
  target_compile_options(${PROJECT_NAME}-sqlgen PRIVATE -g -O0 -fno-omit-frame-pointer -fno-inline)

  add_sanitizers(${PROJECT_NAME}-sqlgen)
else()
  target_compile_options(${PROJECT_NAME}-sqlgen PRIVATE -O2)
endif()

target_include_directories(${PROJECT_NAME}-sqlgen PRIVATE
  $<TARGET_PROPERTY:preludec,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(${PROJECT_NAME}-sqlgen PRIVATE
  preludec
  ${PROJECT_NAME}
)

set(DB_SQLGEN_BIN $<TARGET_FILE:${PROJECT_NAME}-sqlgen> CACHE INTERNAL "Path to database sqlgen tool")
