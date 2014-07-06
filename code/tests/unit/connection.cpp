#include "test.h" // Brings in the UnitTest++ framework
#include "connection.h"

using namespace followermaze;

TEST(ConstructConnection)
{
    Connection conn(9090);
}

TEST(ConstructAsyncConnection)
{
    Connection conn(9090, true);
}

TEST(FailToServeOnSamePort)
{
    Connection conn(9090);
    CHECK_THROW(Connection conn1(9090), Connection::Exception);
}

TEST(ServeOnSamePortTwice)
{
    {
        Connection conn(9090);
    }

    {
        Connection conn(9090);
    }
}

TEST(ReceiveUnacceptedFails)
{
    Connection conn(9090);
    CHECK_THROW(conn.receive(), Connection::Exception);
}

TEST(SendUnacceptedFails)
{
    Connection conn(9090);
    CHECK_THROW(conn.send("bla"), Connection::Exception);
}

TEST(AcceptAsyncFails)
{
    Connection conn(9090, true);
    CHECK_THROW(conn.accept(), Connection::Exception);
}

TEST(ReceiveUnacceptedAsyncFails)
{
    Connection conn(9090, true);
    CHECK_THROW(conn.receive(), Connection::Exception);
}

TEST(SendUnacceptedAsyncFails)
{
    Connection conn(9090, true);
    CHECK_THROW(conn.send("bla"), Connection::Exception);
}

TEST(InvalidMethodInvocationFails)
{
    Connection conn;
    CHECK_EQUAL(conn.getHandle(), INVALID_HANDLE);
    CHECK_THROW(conn.accept(), Connection::Exception);
    CHECK_THROW(conn.receive(), Connection::Exception);
    CHECK_THROW(conn.send("bla"), Connection::Exception);
}
