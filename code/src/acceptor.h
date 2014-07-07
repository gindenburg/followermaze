/* This file describes Acceptor class template (extension of the Reactor pattern).
 * Acceptor is a special event handler which accepts connection requests
 * and creates clients (defined as template parameter) to handle the connection.
 */

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "eventhandler.h"
#include "connection.h"

namespace followermaze
{

class Connection;
class Reactor;

class Acceptor : public EventHandler
{
public:
    Acceptor(int port, Reactor &reactor, EventHandlerFactory &factory);

    virtual Handle getHandle();

    // Accepts connection request, creates the client and registers it
    // with the reactor.
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
