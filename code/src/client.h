#ifndef CLIENT_H
#define CLIENT_H

#include "eventhandler.h"
#include "connection.h"
#include "reactor.h"
#include <memory>
#include <string>

namespace followermaze
{

class Client : public EventHandler
{
protected:
    virtual ~Client();

public:
    Client(auto_ptr<Connection> conn, Reactor &reactor);

    virtual Handle getHandle();
    virtual void handleInput(int hint);
    virtual void handleOutput(int hint);
    virtual void handleClose(int hint);
    virtual void handleError(int hint);

protected:
    virtual void doHandleInput(int hint);
    virtual void doHandleOutput(int hint);

    void disconnect(int hint);

protected:
    auto_ptr<Connection> m_conn;
    Reactor &m_reactor;
};

class Admin : public Client
{
public:
    Admin(auto_ptr<Connection> conn, Reactor &reactor);

protected:
    virtual void doHandleInput(int hint);

protected:
    virtual ~Admin();
};

class EventSource : public Client
{
public:
    EventSource(auto_ptr<Connection> conn, Reactor &reactor);

    virtual void doHandleInput(int hint);

protected:
    virtual ~EventSource();
};

class UserClient : public Client
{
public:
    UserClient(auto_ptr<Connection> conn, Reactor &reactor);

protected:
    virtual void doHandleInput(int hint);
    virtual void doHandleOutput(int hint);

protected:
    virtual ~UserClient();

protected:
    string m_message;
};

} // namespace followermaze

#endif // CLIENT_H
