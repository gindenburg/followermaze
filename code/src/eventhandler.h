/* This file describes EventHandler interface (part of the  Reactor pattern).
 */

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <memory>
#include "connection.h"

namespace followermaze
{

/* EventHandler is a part of Reactor pattern which is responsible
 * for handling events associated with Handle which may occure as a
 * result of an I/O operation.
 * EventHandler is owned by Reactor and called back if an event occures.
 */
class EventHandler
{
public:
    virtual ~EventHandler() {}

    // Return Handle associated with this EventHandler.
    virtual Handle getHandle() = 0;

    // Called if input is available on the Handle.
    // hint should be passed to methods of Reactor.
    virtual void handleInput(int hint)
    {
    }

    // Called if Handle is ready to accept output.
    // hint should be passed to methods of Reactor.
    virtual void handleOutput(int hint)
    {
    }

    // Called if Handle has been closed.
    // hint should be passed to methods of Reactor.
    virtual void handleClose(int hint)
    {
    }

    // Called if timed out while waiting for an event on the Handle.
    // hint should be passed to methods of Reactor.
    virtual void handleTimeout(int hint)
    {
    }

    // Called if an error occured while waiting for an event on Handle.
    // hint should be passed to methods of Reactor.
    virtual void handleError(int hint)
    {
    }
};

class Reactor;

/* EventHandlerFactory is a prototype of a factory to create event haddlers.
 * It is used as a part of Acceptor pattern to inject business logic into the
 * event handlers without contaminating Acceptor with details of business logic
 * implementation.
 */
class EventHandlerFactory
{
public:
    virtual ~EventHandlerFactory() {}

    // Returns new Eventhandler which can be registered with the reactor and handle
    // events associated with the connection.
    virtual EventHandler *createEventHandler(auto_ptr<Connection> connection, Reactor &reactor) = 0;
};

} // namespace followermaze

#endif // EVENTHANDLER_H
