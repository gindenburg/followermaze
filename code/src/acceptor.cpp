#include "acceptor.h"
#include "eventhandler.h"
#include "reactor.h"

namespace followermaze
{

Acceptor::Acceptor(int port, Reactor &reactor, EventHandlerFactory &factory) :
    m_connection(port),
    m_reactor(reactor),
    m_factory(factory)
{
}

Handle Acceptor::getHandle()
{
    return m_connection.getHandle();
}

void Acceptor::handleInput(int /*hint*/)
{
    auto_ptr<Connection> clientConn(m_connection.accept(true));
    auto_ptr<EventHandler> client(m_factory.createEventHandler(clientConn, m_reactor));
    m_reactor.addHandler(client, Reactor::EvntRead);
}

Acceptor::~Acceptor()
{
}

} // namespace followermaze
