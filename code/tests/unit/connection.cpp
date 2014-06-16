#include "test.h" // Brings in the UnitTest++ framework
#include "connection.h"

TEST(constructConnection)
{
    Connection conn(9090);
}

TEST(constructAsyncConnection)
{
    Connection conn(9090, true);
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

TEST(constructedIsAlive)
{
    Connection conn(9090);
    CHECK(conn.isAlive());
}

TEST(constructedAsyncIsAlive)
{
    Connection conn(9090, true);
    CHECK(conn.isAlive());
}

TEST(receiveUnacceptedFails)
{
    Connection conn(9090);
    CHECK_THROW(conn.receive(), Connection::Exception);
}

TEST(sendUnacceptedFails)
{
    Connection conn(9090);
    CHECK_THROW(conn.send("bla"), Connection::Exception);
}

TEST(acceptAsyncFails)
{
    Connection conn(9090, true);
    CHECK_THROW(conn.accept(), Connection::Exception);
}

TEST(receiveUnacceptedAsyncFails)
{
    Connection conn(9090, true);
    CHECK_THROW(conn.receive(), Connection::Exception);
}

TEST(sendUnacceptedAsyncFails)
{
    Connection conn(9090, true);
    CHECK_THROW(conn.send("bla"), Connection::Exception);
}
