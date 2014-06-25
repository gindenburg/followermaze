#
# Build followermaze app
#
# List the source files
set(SRC_LIST
    exception.h
    connection.h
    connection.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

