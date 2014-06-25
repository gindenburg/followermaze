#
# Build multiecho app
#

# Choose app's name
set(APP_NAME "multiecho")

# List the source files
set(SRC_LIST
    ${FOLLOWERMAZE_SOURCE_PATH}/connection.cpp
    main.cpp
)

# Set the main app source location to includes
include_directories(${FOLLOWERMAZE_SOURCE_PATH})

add_executable(${APP_NAME} ${SRC_LIST})
target_link_libraries(${APP_NAME} pthread)

