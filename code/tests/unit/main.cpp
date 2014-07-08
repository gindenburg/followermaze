#include "test.h"
#include "logger.h"

int main()
{
    followermaze::Logger::getInstance().setLogLevel(followermaze::Logger::LvlError);
    return UnitTest::RunAllTests();
}
