project(followermaze)
cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(FOLLOWERMAZE_SOURCE_PATH "${PROJECT_SOURCE_DIR}/src")

# Configure build for unit tests
add_subdirectory(./tests/unit)

# Configure build for the test apps
add_subdirectory(./tests/apps)

# Configure build for the main app
add_subdirectory(./src)
