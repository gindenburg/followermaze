#include "test.h" // Brings in the UnitTest++ framework
#include "engine.h"
#include "reactor.h"
#include <vector>

using namespace std;
using namespace followermaze;

class TestClient : public Protocol::UserClient
{
public:
    vector< string > m_msg;

public:
    TestClient(auto_ptr<Connection> conn, Reactor &reactor, Engine &engine) :
        UserClient(conn, reactor, engine)
    {
    }

    virtual void send(const string &message)
    {
        m_msg.push_back(message);
    }
};

class TestEngine : public Engine
{
public:
    Protocol::User *getUser(long id)
    {
        Protocol::UserMap::iterator userIt = m_users.find(id);
        if (userIt == m_users.end())
        {
            return NULL;
        }

        return userIt->second;
    }

    int eventsQueueing()
    {
        return m_events.size();
    }
};

TEST(RegisterUser)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);

    long id = engine.registerUser(&client, "123456\n");
    CHECK_EQUAL(123456, id);

    Protocol::User *user = engine.getUser(id);
    CHECK(user != NULL);
    CHECK_EQUAL(123456, user->m_id);
    CHECK_EQUAL(1, user->m_clients.size());
    CHECK_EQUAL(user->m_clients.front(), (UserClient*)&client);

    // Test registering the same user twice.
    long id1 = engine.registerUser(&client, "123456\n");
    CHECK(id == id1);
    CHECK_EQUAL(2, user->m_clients.size());

    engine.unregisterUser(id, &client);
    CHECK_EQUAL(1, user->m_clients.size());

    engine.unregisterUser(id1, &client);
    CHECK(NULL == engine.getUser(id));
}

TEST(EventsHandledInOrder)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);

    long id1 = engine.registerUser(&client, "1\n");
    long id2 = engine.registerUser(&client, "2\n");

    string events = "4|B\n2|F|1|2\n5|U|1|2\n1|P|1";
    engine.handleEvents(events);
    CHECK_EQUAL("1|P|1", events);
    events += "|2\n3|S|2\n";
    engine.handleEvents(events);
    CHECK_EQUAL("", events);

    CHECK_EQUAL(5, client.m_msg.size());
    CHECK_EQUAL("1|P|1|2\n", client.m_msg[0]);
    CHECK_EQUAL("2|F|1|2\n", client.m_msg[1]);
    CHECK_EQUAL("3|S|2\n", client.m_msg[2]);
    CHECK_EQUAL("4|B\n", client.m_msg[3]);
    CHECK_EQUAL("4|B\n", client.m_msg[4]);

    engine.unregisterUser(id1, &client);
    engine.unregisterUser(id2, &client);
}

TEST(Follow)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);
    string events;

    // Check that events to unconnected users are ignored.
    events = "1|F|1|2\n";
    engine.handleEvents(events);

    engine.registerUser(&client, "2\n");

    events = "2|F|1|2\n";
    engine.handleEvents(events);
    CHECK_EQUAL("", events);
    CHECK_EQUAL(1, client.m_msg.size());
    CHECK_EQUAL("2|F|1|2\n", client.m_msg[0]);

    Protocol::User *user1 = engine.getUser(1l);
    CHECK(user1 != NULL);
    CHECK_EQUAL(0, user1->m_clients.size());
    CHECK_EQUAL(0, user1->m_followers.size());
    CHECK_EQUAL(1, user1->m_followees.size());

    Protocol::User *user2 = engine.getUser(2l);
    CHECK(user2 != NULL);
    CHECK_EQUAL(1, user2->m_clients.size());
    CHECK_EQUAL(1, user2->m_followers.size());
    CHECK_EQUAL(0, user2->m_followees.size());

    engine.registerUser(&client, "1\n");
    CHECK_EQUAL(1, user1->m_clients.size());
}

TEST(Unfollow)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);
    string events;

    engine.registerUser(&client, "2\n");

    events = "1|F|1|2\n";
    engine.handleEvents(events);

    events = "2|U|1|2\n";
    engine.handleEvents(events);
    CHECK(NULL == engine.getUser(1l));
    Protocol::User *user2 = engine.getUser(2l);
    CHECK(user2 != NULL);
    CHECK_EQUAL(0, user2->m_followers.size());

    events = "3|F|1|2\n";
    engine.handleEvents(events);

    engine.registerUser(&client, "1\n");
    events = "4|U|1|2\n";
    engine.handleEvents(events);
    Protocol::User *user1 = engine.getUser(1l);
    CHECK(user1 != NULL);
    CHECK_EQUAL(0, user1->m_followees.size());
}

TEST(Private)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);

    // Check that events to unconnected users are ignored.
    string events = "1|P|1|2\n";
    engine.handleEvents(events);
    CHECK_EQUAL(0, engine.eventsQueueing());

    long id = engine.registerUser(&client, "2\n");

    events = "2|P|1|2\n";
    engine.handleEvents(events);
    CHECK_EQUAL("", events);
    CHECK_EQUAL(1, client.m_msg.size());
    CHECK_EQUAL("2|P|1|2\n", client.m_msg[0]);
}

TEST(StatusUpdate)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);
    string events;

    engine.registerUser(&client, "3\n");
    events = "1|F|1|3\n2|F|2|3\n";
    engine.handleEvents(events);

    // Check that events to unconnected users are ignored.
    events = "3|S|3\n";
    engine.handleEvents(events);
    CHECK_EQUAL(0, engine.eventsQueueing());

    // Now connect them and try again
    engine.registerUser(&client, "1\n");
    engine.registerUser(&client, "2\n");
    events = "4|S|3\n";
    engine.handleEvents(events);
    CHECK_EQUAL(4, client.m_msg.size());
    CHECK_EQUAL("1|F|1|3\n", client.m_msg[0]);
    CHECK_EQUAL("2|F|2|3\n", client.m_msg[1]);
    CHECK_EQUAL("4|S|3\n", client.m_msg[2]);
    CHECK_EQUAL("4|S|3\n", client.m_msg[3]);
}

TEST(Broadcast)
{
    Reactor reactor;
    TestEngine engine;
    TestClient client(auto_ptr<Connection>(new Connection()), reactor, engine);

    engine.registerUser(&client, "1\n");
    engine.registerUser(&client, "2\n");
    engine.registerUser(&client, "3\n");

    string events = "1|B\n";
    engine.handleEvents(events);
    CHECK_EQUAL("", events);
    CHECK_EQUAL(3, client.m_msg.size());
    CHECK_EQUAL("1|B\n", client.m_msg[0]);
    CHECK_EQUAL("1|B\n", client.m_msg[1]);
    CHECK_EQUAL("1|B\n", client.m_msg[2]);
}
