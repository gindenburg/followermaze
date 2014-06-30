#
# Build followermaze app
#
# List the source files
set(SRC_LIST
    exception.h
    logger.h
    logger.cpp
    connection.h
    connection.cpp
    eventhandler.h
    eventhandler.cpp
    client.h
    client.cpp
    acceptor.h
    reactor.h
    reactor.cpp
    server.h
    server.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
