/* This file defines the Client interface and it's specialisation Admin.
 * It also contains declaration of EventHandlerFactory.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "eventhandler.h"
#include "connection.h"
#include <memory>
#include <string>

namespace followermaze
{

class Reactor;

/*
 * Client is a base class for all clients. Client implements EventHandler
 * interface to introduce basic functionality and error handling.
 * Client encapsulates a Connection and uses Reactor to handle events.
 * Client instances normally are created by Acceptor, owned by Reactor and
 * disposed of in call back functions while hadling client disconnect or error.
 */
class Client : public EventHandler
{
public:
    Client(auto_ptr<Connection> conn, Reactor &reactor);

    // Implement EventHandler  interface.
    virtual Handle getHandle();
    virtual void handleInput(int hint);
    virtual void handleOutput(int hint);
    virtual void handleClose(int hint);
    virtual void handleError(int hint);

protected:
    // handleInput and handleOutput are implemented as template methods
    // which define error handling around doHandleInput/doHandleOutput
    // which should be overridden by the subclasses to do the processing.
    virtual void doHandleInput(int hint);
    virtual void doHandleOutput(int hint);

    // Unregister this from the Reactor and delete this.
    void dispose(int hint);

protected:
    // Ensure dynamic allocation.
    virtual ~Client();

protected:
    auto_ptr<Connection> m_connection;
    Reactor &m_reactor;
};

/*
 * Admin is a client which can be used to interrupt Reactor's event loop.
 */
class Admin : public Client
{
public:
    Admin(auto_ptr<Connection> conn, Reactor &reactor);

protected:
    // Stop Reactor if received "stop".
    virtual void doHandleInput(int hint);

protected:
    // Ensure dynamic allocation.
    virtual ~Admin();
};

/*
 * ClientFactory is a template interface which declares a factory method
 * to create EventHandler. ClientType is expected to have a constructor
 * which accepts auto_ptr<Connection> and reference to Reactor.
 * This class (or subclasses) must be instantiated to initialize Acceptor.
 * Acceptor will use the instances of this class to create Clients.
 */
template < class ClientType >
class ClientFactory : public EventHandlerFactory
{
public:
    virtual EventHandler *createEventHandler(auto_ptr<Connection> connection, Reactor &reactor)
    {
        return new ClientType(connection, reactor);
    }
};

} // namespace followermaze

#endif // CLIENT_H
