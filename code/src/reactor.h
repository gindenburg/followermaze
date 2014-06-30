#ifndef REACTOR_H
#define REACTOR_H

#include <poll.h>
#include <memory>
#include "exception.h"
#include "eventhandler.h"

namespace followermaze
{

class Reactor
{
public:
    class Exception : public BaseException
    {
    public:
        enum
        {
            ErrBusy = BaseException::ErrGeneric + 1,
            ErrHandleDuplicate,
            ErrStop
        };

        Exception(int err = BaseException::ErrGeneric);
        virtual const char* what() const throw();
    };

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

    void addHandler(auto_ptr<EventHandler> handler, EventType event);
    void resetHandler(int hint, EventType event);
    EventHandler* detouchHandler(int hint);
    void handleEvents();

protected:
    static const unsigned int MAX_FDS = 128;
    struct pollfd m_pollfds[MAX_FDS];
    EventHandler *m_handlers[MAX_FDS];

};

} // namespace followermaze

#endif // REACTOR_H
