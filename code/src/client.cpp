#include <assert.h>
#include "client.h"
#include "reactor.h"
#include "engine.h"
#include "logger.h"

namespace followermaze
{

Client::~Client()
{
}

Client::Client(auto_ptr<Connection> conn, Reactor &reactor, Engine &engine) :
    m_connection(conn),
    m_reactor(reactor),
    m_engine(engine)
{
    assert(m_connection.get() != NULL);
}

Handle Client::getHandle()
{
    return m_connection->getHandle();
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
            dispose(hint);
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
            dispose(hint);
        }
        else
        {
            throw e;
        }
    }
}

void Client::handleClose(int hint)
{
    dispose(hint);
}

void Client::handleError(int hint)
{
    Logger::getInstance().message("Client error.");
    dispose(hint);
}

void Client::doHandleInput(int hint)
{
}

void Client::doHandleOutput(int hint)
{
}

void Client::dispose(int hint)
{
    EventHandler* self = m_reactor.detouchHandler(hint);
    assert(self == (EventHandler*)this);
    delete this;
    Logger::getInstance().message("Client disconnected.");
}

Admin::Admin(auto_ptr<Connection> conn, Reactor &reactor, Engine &engine) :
    Client(conn, reactor, engine)
{
}

Admin::~Admin()
{
}

void Admin::doHandleInput(int hint)
{
    string command = m_connection->receive();

    if (command.compare(0, 4, "stop") == 0)
    {
        Logger::getInstance().message("Got stop command.");
        throw Reactor::Exception(Reactor::Exception::ErrStop);
    }
}

} // namespace followermaze
