#
# Build and run unit tests
#
set(UNIT_TESTS_RUNNER ${PROJECT_NAME}_run_unit_tests)

set(SRC_LIST
    test.h
    connection.cpp
    sanity_check.cpp
    main.cpp
)

include_directories(${FOLLOWERMAZE_SOURCE_PATH})
add_executable(${UNIT_TESTS_RUNNER} ${SRC_LIST})
target_link_libraries(${UNIT_TESTS_RUNNER} libunittest++.a)

add_custom_target(check ALL "${EXECUTABLE_OUTPUT_PATH}/${UNIT_TESTS_RUNNER}" DEPENDS ${UNIT_TESTS_RUNNER} COMMENT "Running unit tests..." VERBATIM)
add_custom_target(test "${EXECUTABLE_OUTPUT_PATH}/${UNIT_TESTS_RUNNER}" DEPENDS ${UNIT_TESTS_RUNNER} COMMENT "Running unit tests..." VERBATIM)
