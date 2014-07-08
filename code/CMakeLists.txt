project(followermaze)
cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(FOLLOWERMAZE_SOURCE_PATH "${PROJECT_SOURCE_DIR}/src")
set(FOLLOWERMAZE_LIBRARY_NAME ${PROJECT_NAME}lib)

set(WARNINGS "-Wall -Wextra -Werror")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNINGS}")

# Configure build for the main app
add_subdirectory(./src)

# Configure build for unit tests
add_subdirectory(./tests/unit)

# Configure build for the test apps
add_subdirectory(./tests/apps)
