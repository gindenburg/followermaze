#
# Build echo app
#

# Choose app's name
set(ECHO_APP "echo")

# Set the main app source location to includes
include_directories(${FOLLOWERMAZE_SOURCE_PATH})

add_executable(${ECHO_APP} main.cpp)
target_link_libraries(${ECHO_APP} ${FOLLOWERMAZE_LIBRARY_NAME})
