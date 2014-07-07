#
# Build multiecho app
#

# Choose app's name
set(APP_NAME "multiecho")

# Set the main app source location to includes
include_directories(${FOLLOWERMAZE_SOURCE_PATH})

add_executable(${APP_NAME} main.cpp)
target_link_libraries(${APP_NAME} ${FOLLOWERMAZE_LIBRARY_NAME})
