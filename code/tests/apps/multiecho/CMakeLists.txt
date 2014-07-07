#
# Build multiecho app
#

# Choose app's name
set(APP_NAME "multiecho")

# List the source files
set(SRC_LIST
    ${FOLLOWERMAZE_SOURCE_PATH}/acceptor.cpp
    ${FOLLOWERMAZE_SOURCE_PATH}/client.cpp
    ${FOLLOWERMAZE_SOURCE_PATH}/connection.cpp
    ${FOLLOWERMAZE_SOURCE_PATH}/logger.cpp
    ${FOLLOWERMAZE_SOURCE_PATH}/reactor.cpp
    ${FOLLOWERMAZE_SOURCE_PATH}/server.cpp
    main.cpp
)

# Set the main app source location to includes
include_directories(${FOLLOWERMAZE_SOURCE_PATH})

add_executable(${APP_NAME} ${SRC_LIST})
target_link_libraries(${APP_NAME} pthread)

