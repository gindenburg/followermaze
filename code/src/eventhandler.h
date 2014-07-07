/* This file describes EventHandler interface (part of the  Reactor pattern).
 */

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <memory>
#include "connection.h"

namespace followermaze
{

class EventHandler
{
public:
    virtual ~EventHandler() {}

    virtual Handle getHandle() = 0;

    virtual void handleInput(int hint)
    {
    }

    virtual void handleOutput(int hint)
    {
    }

    virtual void handleClose(int hint)
    {
    }

    virtual void handleTimeout(int hint)
    {
    }

    virtual void handleError(int hint)
    {
    }
};

class Reactor;

class EventHandlerFactory
{
public:
    virtual ~EventHandlerFactory() {}

    virtual EventHandler *createEventHandler(auto_ptr<Connection> connection, Reactor &reactor) = 0;
};

} // namespace followermaze

#endif // EVENTHANDLER_H
