#include "test.h"
#include "logger.h"

int main(int argc, char* argv[])
{
    followermaze::Logger::getInstance().setLogLevel(followermaze::Logger::LvlError);
    return UnitTest::RunAllTests();
}
