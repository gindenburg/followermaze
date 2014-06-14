project(followermaze)
cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

set(UNIT_TESTS_RUNNER ${PROJECT_NAME}_run_unit_tests)

aux_source_directory(./tests/unit TSTSRC_LIST)
add_executable(${UNIT_TESTS_RUNNER} ${TSTSRC_LIST})
target_link_libraries(${UNIT_TESTS_RUNNER} libunittest++.a)

add_custom_target(check ALL "${EXECUTABLE_OUTPUT_PATH}/${UNIT_TESTS_RUNNER}" DEPENDS ${UNIT_TESTS_RUNNER} COMMENT "Running unit tests..." VERBATIM)
add_custom_target(test "${EXECUTABLE_OUTPUT_PATH}/${UNIT_TESTS_RUNNER}" DEPENDS ${UNIT_TESTS_RUNNER} COMMENT "Running unit tests..." VERBATIM)

aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
