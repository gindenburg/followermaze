#include <assert.h>
#include "client.h"
#include "logger.h"

namespace followermaze
{

Client::~Client()
{
}

Client::Client(auto_ptr<Connection> conn, Reactor &reactor) :
    m_conn(conn), m_reactor(reactor)
{
    assert(m_conn.get() != NULL);
}

Handle Client::getHandle()
{
    return m_conn->getHandle();
}

void Client::handleInput(int hint)
{
    try
    {
        doHandleInput(hint);
    }
    catch (Connection::Exception e)
    {
        if (e.getErr() == Connection::Exception::ErrClientDisconnect)
        {
            disconnect(hint);
        }
        else
        {
            throw e;
        }
    }
}

void Client::handleOutput(int hint)
{
    try
    {
        doHandleOutput(hint);
    }
    catch (Connection::Exception e)
    {
        if (e.getErr() == Connection::Exception::ErrClientDisconnect)
        {
            disconnect(hint);
        }
        else
        {
            throw e;
        }
    }
}

void Client::handleClose(int hint)
{
    disconnect(hint);
}

void Client::handleError(int hint)
{
    Logger::getInstance().message("Client error.");
    disconnect(hint);
}

void Client::doHandleInput(int hint)
{
}

void Client::doHandleOutput(int hint)
{
}

void Client::disconnect(int hint)
{
    EventHandler* self = m_reactor.detouchHandler(hint);
    assert(self == (EventHandler*)this);
    delete this;
    Logger::getInstance().message("Client disconnected.");
}

Admin::Admin(auto_ptr<Connection> conn, Reactor &reactor) :
    Client(conn, reactor)
{
}

Admin::~Admin()
{
}

void Admin::doHandleInput(int hint)
{
    string command = m_conn->receive();

    if (command.compare(0, 4, "stop") == 0)
    {
        Logger::getInstance().message("Got stop command.");
        throw Reactor::Exception(Reactor::Exception::ErrStop);
    }
}

EventSource::EventSource(auto_ptr<Connection> conn, Reactor &reactor) :
    Client(conn, reactor)
{
}

EventSource::~EventSource()
{
}

void EventSource::doHandleInput(int hint)
{
}

UserClient::UserClient(auto_ptr<Connection> conn, Reactor &reactor) :
    Client(conn, reactor)
{
}

UserClient::~UserClient()
{
}

void UserClient::doHandleInput(int hint)
{
    m_message += m_conn->receive();
    m_reactor.resetHandler(hint, (Reactor::EvntWrite | Reactor::EvntRead));
}

void UserClient::doHandleOutput(int hint)
{
    m_conn->send("message");
    m_reactor.resetHandler(hint, Reactor::EvntRead);
}

} // namespace followermaze
