/* This file declears Reactor class
 */
#ifndef REACTOR_H
#define REACTOR_H

#include <poll.h>
#include <memory>
#include "exception.h"
#include "eventhandler.h"

namespace followermaze
{

/* Reactor implements a part of Reactor pattern for synchronous I/O event
 * demultiplexing.
 * Reactor owns event handlers and disposes of them at destruction unless
 * disposed of as a reaction to an event.
 * This implementation uses poll mechanism and hard-coded (1024) amount of
 * descriptors. Obviously, implementations using single instance of this
 * Reactor will be limited to 1024 files (sockets). This is probably a
 * reasonable limitation for a poll based implementation. poll is relatively
 * slow and gets slower linearly with the number of descriptors.
 * Implementations aiming at handling thousands of concurrent connections
 * could use multiple server threads each using its own Reactor. This would
 * require Reactor to be made thread safe.
 * Better option would be to use event based dispatching (e.g. epoll, kqueue),
 * but it's less portable and makes code more compicated.
 */
class Reactor
{
public:
    class Exception : public BaseException
    {
    public:
        enum
        {
            ErrBusy = BaseException::ErrGeneric + 1, // No more slots
            ErrHandleDuplicate, // Trying to register the same handle twice
            ErrStop // Stop the reaction (should be thrown by an EventHandler)!
        };

        Exception(int err = BaseException::ErrGeneric) : BaseException(err) {}
        virtual const char* what() const throw() { return "Reactor::Exception#"; }
    };

    // Supported event types (can be OR-ed)
    typedef unsigned int EventType;
    enum
    {
        EvntAccept = 0x01,
        EvntRead = 0x01,
        EvntWrite = 0x02
    };

public:
    Reactor();
    virtual ~Reactor();

    // Registers the handler (takes ownership) to handle event.
    // Will throw if handler is NULL, a handler with the same Handle
    // has been already registered, or out of room.
    void addHandler(auto_ptr<EventHandler> handler, EventType event);

    // Makes a handler which has been called back with the hint to handle event.
    void resetHandler(int hint, EventType event);

    // Deregister an event handler which has been called back with the hint.
    // Ownership is passed to the caller.
    EventHandler* detouchHandler(int hint);

    // Waits for events and dispatches them to the EventHandler's callbacks.
    // Throws on poll error. Passes through all exceptions from EventHandlers.
    void handleEvents();

protected:
    static const int MAX_FDS = 1024;
    struct pollfd m_pollfds[MAX_FDS];
    EventHandler *m_handlers[MAX_FDS];
};

} // namespace followermaze

#endif // REACTOR_H
