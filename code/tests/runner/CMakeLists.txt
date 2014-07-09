configure_file(template testrunner.sh)

# followermaze CLI tests
add_test(NAME TestCLIHelp COMMAND $<TARGET_FILE:${PROJECT_NAME}> -h)
add_test(NAME TestCLIHelpLong COMMAND $<TARGET_FILE:${PROJECT_NAME}> --help)
set_tests_properties(TestCLIHelp TestCLIHelpLong PROPERTIES PASS_REGULAR_EXPRESSION "Usage")

add_test(NAME TestCLIInvalidCommand COMMAND $<TARGET_FILE:${PROJECT_NAME}> bla)
set_tests_properties(TestCLIInvalidCommand PROPERTIES PASS_REGULAR_EXPRESSION "Invalid command: bla")

add_test(NAME TestCLISmallEventPort COMMAND $<TARGET_FILE:${PROJECT_NAME}> 1000 9090)
set_tests_properties(TestCLISmallEventPort PROPERTIES PASS_REGULAR_EXPRESSION "Invalid event_source_port: 1000")

add_test(NAME TestCLILargeEventPort COMMAND $<TARGET_FILE:${PROJECT_NAME}> 65536 9090)
set_tests_properties(TestCLILargeEventPort PROPERTIES PASS_REGULAR_EXPRESSION "Invalid event_source_port: 65536")

add_test(NAME TestCLISmallClientPort COMMAND $<TARGET_FILE:${PROJECT_NAME}> 9090 1000)
set_tests_properties(TestCLISmallClientPort PROPERTIES PASS_REGULAR_EXPRESSION "Invalid user_client_port: 1000")

add_test(NAME TestCLILargeClientPort COMMAND $<TARGET_FILE:${PROJECT_NAME}> 9090 65536)
set_tests_properties(TestCLILargeClientPort PROPERTIES PASS_REGULAR_EXPRESSION "Invalid user_client_port: 65536")

add_test(NAME TestCLIStop COMMAND $<TARGET_FILE:${PROJECT_NAME}> stop)

# Tests using the testsuite
add_test(NAME SmokeTest10KEvents100Clients COMMAND "./testrunner.sh" $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(SmokeTest10KEvents100Clients PROPERTIES ENVIRONMENT "totalEvents=10000;concurrencyLevel=100")

add_test(NAME Test1EventPerBatch COMMAND "./testrunner.sh" $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(Test1EventPerBatch PROPERTIES ENVIRONMENT "totalEvents=10000;concurrencyLevel=100;maxEventSourceBatchSize=1")

add_test(NAME Test1Client COMMAND "./testrunner.sh" $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(Test1Client PROPERTIES ENVIRONMENT "totalEvents=10000;concurrencyLevel=100;maxEventSourceBatchSize=1")

add_test(NAME UltimateTestAllDefaults_VERY_LONG COMMAND "./testrunner.sh" $<TARGET_FILE:${PROJECT_NAME}>)

set_tests_properties(SmokeTest10KEvents100Clients Test1EventPerBatch Test1Client UltimateTestAllDefaults_VERY_LONG
                     PROPERTIES FAIL_REGULAR_EXPRESSION "SOMETHING WENT WRONG")
