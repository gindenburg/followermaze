#
# Build followermaze app
#
# List the source files
set(SRC_LIST
    acceptor.h
    engine.h
    engine.cpp
    eventhandler.h
    exception.h
    client.h
    client.cpp
    connection.h
    connection.cpp
    logger.h
    logger.cpp
    protocol.h
    protocol.cpp
    reactor.h
    reactor.cpp
    server.h
    server.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
