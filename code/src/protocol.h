/* This file declears a family of classes and types encapsulating followermaze
 * application protocol. The following classes and types are decleared:
 *  Event
 *  EventQueue
 *  User
 *  UserMap
 *  EventSource
 *  UserClient
 *  ClientList
 *  ClientFactory
 *  Parser
 *  Engine (only forward declaration, declared in engine.h)
 */
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

/* Event represents an ennumerated event of defined type which can be used to
 * send a message (its payload) from one user to one or many other users.
 */
struct Event
{
    string m_payload;
    long m_seqnum;
    char m_type;
    long m_fromUserId;
    long m_toUserId;

    /* order_by_seqnum_ascending is a functor which defines strict weak
     * ordering for Event pointers (by m_seqnum descending).
     */
    struct order_by_seqnum_descending
    {
        bool operator() (const Event *lhs, const Event *rhs) const
        {
            return lhs->m_seqnum > rhs->m_seqnum;
        }
    };
};

/* EventSource is a client which receives events and uses Engine
 * to process them. It adds a part (event processing) of followermaze business
 * logic to the Reactor pattern.
 */
class EventSource : public Client
{
public:
    // Creates an event source. Takes ownership over connection.
    EventSource(auto_ptr<Connection> connection, Reactor &reactor, Engine &engine);

    // Implement EventHandler interface.
    virtual void handleClose(int hint);
    virtual void handleError(int hint);

protected:
    // Implement event source specific input processing.
    virtual void doHandleInput(int hint);

protected:
    // Ensure dynamic allocation.
    virtual ~EventSource();

protected:
    Engine &m_engine;
    string m_buffer; // internal buffer for the incoming data
};

/* User map maps user ID to the pointer to a User instance.
 */
struct User;
typedef map< long, User* > UserMap;

/* ClientList is a list of UserClient pointers
 */
class UserClient;
typedef list< UserClient* > ClientList;

/* User represents a user which is identified by an ID, can connect from
 * multiple clients, follow other users, and be followed by other users.
 */
struct User
{
    long m_id;
    UserMap m_followers;
    UserMap m_followees;
    ClientList m_clients;
};

/* UserClient is a client which receives authentication (user ID) and can be
 * used to send messages to the user. It adds a part (user
 * registering/unregistering, and notification) of followermaze business logic
 * to the Reactor pattern.
 */
class UserClient : public Client
{
public:
    // Creates a user client. Takes ownership over connection.
    UserClient(auto_ptr<Connection> connection, Reactor &reactor, Engine &engine);

    // Implement EventHandler interface.
    virtual void handleClose(int hint);
    virtual void handleError(int hint);

    // Sends a message to the user on the other end of the connection.
    virtual void send(const string& message);

protected:
    // Implement user client specific input/output processing.
    virtual void doHandleInput(int hint);
    virtual void doHandleOutput(int hint);

    // Unregister already registered user and cleanup the state.
    void reset(int hint);

protected:
    virtual ~UserClient();

protected:
    Engine &m_engine;
    string m_messageIn;  // internal buffer for the incoming message
    string m_messageOut; // internal buffer for the outgoing message
    long m_userId;       // cached registered user ID
    int m_hint;          // cached Reactor hint to notify user outside EventHandler callbacks.
};

/* EventQueue is a priority queue of events with an event with smallest
 * sequence number on the top.
 */
typedef priority_queue< Event*, vector< Event* >, Event::order_by_seqnum_descending > EventQueue;

/* Parser defines followermaze protocol syntax and implements parsing and
 * encoding for the protocol tokens.
 */
struct Parser
{
    // Finds a single message in a CRLF separated string starting at start.
    // Returns true if message found, the message, and start of the next
    // message (string::npos if no more messages).
    static bool findMessage(const string &str, size_t &start, string &message);

    // Parses long from the string.
    // Returns INVALID_LONG if unsecsessful.
    // WARNING! 0 is an invalid long in followermaze.
    static long parseLong(const string &str);

    // Parses event.m_payload and fills in the event.
    static void parseEvent(Event &event);

    // Returns true if event is valid according to the followermaze protocol.
    static bool isValidEvent(const Event &event);

    // Turns payload into a message (LF terminated) which can be sent to a user.
    static void encodeMessage(const string &payload, string &message);

    // Some constants for parsing.
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

/*
 * EngineDrivenClientFactory is a template which declares a factory method to
 * create objects of type ClientType. ClientType is expected to be derived from
 * EventHandler, and to have a constructor which accepts auto_ptr<Connection>,
 * reference to Reactor, and reference to Engine.
 * This class (or subclasses) must be instantiated to initialize Acceptor.
 * Acceptor will use the instances of this class to create clients like
 * EventSource or UserClient.
 */
template< class ClientType >
class EngineDrivenClientFactory : public EventHandlerFactory
{
public:
    // Should be initialized with a reference to Engine to be able to create
    // Engine driven clients.
    EngineDrivenClientFactory(Engine &engine) :
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
