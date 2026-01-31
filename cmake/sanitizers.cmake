set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/external/sanitizers/cmake" ${CMAKE_MODULE_PATH})

set(SANITIZE_ADDRESS ON)
set(SANITIZE_UNDEFINED ON)
find_package(Sanitizers)
