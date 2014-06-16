#include "test.h" // Brings in the UnitTest++ framework
#include "connection.h"

TEST(constructConnection)
{
    Connection conn(9090);
}

TEST(failToServeOnSamePort)
{
    Connection conn(9090);
    CHECK_THROW(Connection conn1(9090), Connection::Exception);
}

TEST(serveOnSamePortTwice)
{
    {
        Connection conn(9090);
    }

    {
        Connection conn(9090);
    }
}
