/* This file describes EventHandler interface (part of the  Reactor pattern).
 */

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

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

} // namespace followermaze

#endif // EVENTHANDLER_H
