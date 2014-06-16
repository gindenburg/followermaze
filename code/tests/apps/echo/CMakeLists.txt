#
# Build echo app
#

# Choose app's name
set(ECHO_APP "echo")

# List the source files
set(SRC_LIST
    ${FOLLOWERMAZE_SOURCE_PATH}/connection.cpp
    main.cpp
)

# Set the main app source location to includes
include_directories(${FOLLOWERMAZE_SOURCE_PATH})

add_executable(${ECHO_APP} ${SRC_LIST})
