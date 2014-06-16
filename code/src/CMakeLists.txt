#
# Build followermaze app
#
# List the source files
set(SRC_LIST
    connection.h
    connection.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

