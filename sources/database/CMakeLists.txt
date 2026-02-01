project(database C)

file(GLOB_RECURSE DATABASE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(${PROJECT_NAME} STATIC ${DATABASE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
