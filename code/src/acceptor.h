/* This file describes Acceptor class template (extension of the Reactor pattern).
 */

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "eventhandler.h"
#include "connection.h"

namespace followermaze
{

class Reactor;

/*
 * Acceptor is an event handler which accepts connection requests, creates
 * clients (using an EventHandlerFactory), and registers them to the Reactor.
 */
class Acceptor : public EventHandler
{
public:
    // Create server connection and listen for clients.
    Acceptor(int port, Reactor &reactor, EventHandlerFactory &factory);

    // Implementation of EventHandler interface.
    virtual Handle getHandle();
    virtual void handleInput(int hint);

protected:
    // Ensure dynamic allocation.
    virtual ~Acceptor();

protected:
    Connection m_connection;
    Reactor &m_reactor;
    EventHandlerFactory &m_factory;
};

} // namespace followermaze

#endif // ACCEPTOR_H
