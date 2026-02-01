project(mudirc-game C)

file(GLOB_RECURSE GAME_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable(${PROJECT_NAME} ${GAME_SOURCES})
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
