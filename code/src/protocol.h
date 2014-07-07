#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <map>
#include <list>
#include <queue>
#include "client.h"

using namespace std;

namespace followermaze
{

class Reactor;

namespace protocol
{

class Engine;

struct Event
{
    string m_payload;
    long m_seqnum;
    char m_type;
    long m_fromUserId;
    long m_toUserId;

    struct order_by_seqnum_ascending
    {
        bool operator() (const Event *lhs, const Event *rhs) const
        {
            return lhs->m_seqnum > rhs->m_seqnum;
        }
    };
};

class EventSource : public Client
{
public:
    EventSource(auto_ptr<Connection> conn, Reactor &reactor, Engine &engine);

    virtual void handleClose(int hint);
    virtual void handleError(int hint);

protected:
    virtual void doHandleInput(int hint);

protected:
    virtual ~EventSource();

protected:
    Engine &m_engine;
    string m_buffer;
};

struct User;
typedef map< long, User* > UserMap;

class UserClient;
typedef list< UserClient* > ClientList;

struct User
{
    long m_id;
    UserMap m_followers;
    UserMap m_followees;
    ClientList m_clients;
};

class UserClient : public Client
{
public:
    UserClient(auto_ptr<Connection> conn, Reactor &reactor, Engine &engine);

    virtual void handleClose(int hint);
    virtual void handleError(int hint);

    virtual void send(const string& message);

protected:
    virtual void doHandleInput(int hint);
    virtual void doHandleOutput(int hint);

    void reset(int hint);

protected:
    virtual ~UserClient();

protected:
    Engine &m_engine;
    string m_messageIn;
    string m_messageOut;
    long m_userId;
    int m_hint;
};

typedef priority_queue< Event*, vector< Event* >, Event::order_by_seqnum_ascending > EventQueue;

struct Parser
{
    static bool findMessage(const string &str, size_t &start, string &message);
    static long parseLong(const string &str);
    static void parseEvent(Event &event);
    static bool isValidEvent(const Event &event);
    static void encodeMessage(const string& payload, string &message);

    static const long FIRST_SEQNUM = 1l;
    static const long INVALID_LONG = -1l;
    static const char CR = '\r';
    static const char LF = '\n';
    static const char *CRLF;
    static const char DELIMITER = '|';
    static const char TYPE_FOLLOW = 'F';
    static const char TYPE_UNFOLLOW = 'U';
    static const char TYPE_BROADCAST = 'B';
    static const char TYPE_PRIVATE = 'P';
    static const char TYPE_STATUSUPDATE = 'S';
    static const char TYPE_INVALID = '.';
};

template< class ClientType >
class ClientFactory : public EventHandlerFactory
{
public:
    ClientFactory(Engine &engine) :
        m_engine(engine)
    {
    }

    virtual EventHandler *createEventHandler(auto_ptr<Connection> connection, Reactor &reactor)
    {
        return new ClientType(connection, reactor, m_engine);
    }

private:
    Engine &m_engine;
};

} // namespace protocol

} // namespace followerspace

#endif // PROTOCOL_H
