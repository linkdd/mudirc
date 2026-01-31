project(preludec C)

file(GLOB_RECURSE PRELUDEC_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(${PROJECT_NAME} STATIC ${PRELUDEC_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Wextra -Werror
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
  target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -fno-omit-frame-pointer -fno-inline)

  add_sanitizers(${PROJECT_NAME})
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/priv
)
