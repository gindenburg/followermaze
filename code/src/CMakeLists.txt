#
# Build followermaze app
#
# List the source files
set(SRC_LIST
    acceptor.h
    acceptor.cpp
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
)

add_library(${FOLLOWERMAZE_LIBRARY_NAME} ${SRC_LIST})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${FOLLOWERMAZE_LIBRARY_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Testing
