/* This file describes Acceptor class template (extension of the Reactor pattern).
 * Acceptor is a special event handler which accepts connection requests
 * and creates clients (defined as template parameter) to handle the connection.
 */

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "eventhandler.h"
#include "reactor.h"
#include "engine.h"
#include "logger.h"

namespace followermaze
{

template < class ClientType >
class Acceptor : public EventHandler
{
public:
    Acceptor(int port, Reactor &reactor, Engine &engine) :
        m_connection(port),
        m_reactor(reactor),
        m_engine(engine)
    {
    }

    virtual Handle getHandle()
    {
        return m_connection.getHandle();
    }

    // Accepts connection request, creates the client and registers it
    // with the reactor.
    virtual void handleInput(int /*hint*/)
    {
        auto_ptr<Connection> clientConn(m_connection.accept(true));
        auto_ptr<EventHandler> client(new ClientType(clientConn, m_reactor, m_engine));

        m_reactor.addHandler(client, Reactor::EvntRead);

        Logger::getInstance().message("Client connected.");
    }

protected:
    // Ensure dynamic allocation.
    virtual ~Acceptor() {}

protected:
    Connection m_connection;
    Reactor &m_reactor;
    Engine &m_engine;
};

} // namespace followermaze

#endif // ACCEPTOR_H
