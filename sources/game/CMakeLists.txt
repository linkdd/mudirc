project(mudirc-game C)

file(GLOB_RECURSE GAME_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

file(GLOB_RECURSE GAME_SQL_INPUT
  ${CMAKE_CURRENT_SOURCE_DIR}/sql/*.sql
)

set(GAME_SQL_OUTPUT_SRC ${CMAKE_CURRENT_BINARY_DIR}/src/generated/sql.c)
set(GAME_SQL_OUTPUT_HDR ${CMAKE_CURRENT_BINARY_DIR}/include/generated/sql.h)
set(GAME_SQL_OUTPUT ${GAME_SQL_OUTPUT_HDR} ${GAME_SQL_OUTPUT_SRC})

add_custom_command(
  OUTPUT ${GAME_SQL_OUTPUT}
  COMMAND ${CMAKE_COMMAND} -E make_directory
    $<SHELL_PATH:${CMAKE_CURRENT_BINARY_DIR}/src/generated>
    $<SHELL_PATH:${CMAKE_CURRENT_BINARY_DIR}/include/generated>
  COMMAND ${DB_SQLGEN_BIN} ${GAME_SQL_OUTPUT} ${GAME_SQL_INPUT}
  DEPENDS ${GAME_SQL_INPUT} database-sqlgen
  COMMENT "Generating SQL queries"
)

add_executable(${PROJECT_NAME} ${GAME_SOURCES} ${GAME_SQL_OUTPUT_SRC})

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

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/include

  $<TARGET_PROPERTY:preludec,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:ircbot,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  preludec
  ircbot
  database
)

install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)
