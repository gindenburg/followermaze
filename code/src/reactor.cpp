#include <errno.h>
#include <assert.h>
#include "reactor.h"

namespace followermaze
{

Reactor::Exception::Exception(int err /*= BaseException::ErrGeneric*/) :
    BaseException(err)
{
}

const char* Reactor::Exception::what() const throw()
{
    return "Reactor::Exception";
}

Reactor::Reactor()
{
    for (unsigned int i = 0; i < MAX_FDS; ++i)
    {
        m_pollfds[i].fd = -1;
        m_pollfds[i].events = 0;
        m_pollfds[i].revents = 0;
        m_handlers[i] = NULL;
    }
}

Reactor::~Reactor()
{
    for (unsigned int i = 0; i < MAX_FDS; ++i)
    {
        delete m_handlers[i];
    }
}

void Reactor::addHandler(auto_ptr<EventHandler> handler, EventType event)
{
    if (handler.get() == NULL)
    {
        throw Exception();
    }

    int handle = static_cast<int>(handler->getHandle());
    int freeSlot = MAX_FDS;

    for (unsigned int i = 0; i < MAX_FDS; ++i)
    {
        if (m_pollfds[i].fd < 0)
        {
            freeSlot = i;
            break;
        }

        if (m_pollfds[i].fd == handle)
        {
            throw Exception(Exception::ErrHandleDuplicate);
        }
    }

    if (freeSlot == MAX_FDS)
    {
        throw Exception(Exception::ErrBusy);
    }

    m_pollfds[freeSlot].fd = handle;
    m_handlers[freeSlot] = handler.release();
    m_pollfds[freeSlot].revents = 0;
    resetHandler(freeSlot, event);
}

void Reactor::resetHandler(int hint, EventType event)
{
    if (hint < 0 || hint >= MAX_FDS)
    {
        throw Exception();
    }

    m_pollfds[hint].events = 0;

    if ((event & EvntAccept) || (event & EvntRead))
    {
        m_pollfds[hint].events |= POLLIN;
    }

    if (event & EvntWrite)
    {
        m_pollfds[hint].events |= POLLOUT;
    }
}

EventHandler* Reactor::detouchHandler(int hint)
{
    if (hint < 0 || hint >= MAX_FDS)
    {
        throw Exception();
    }

    m_pollfds[hint].fd = -1;
    m_pollfds[hint].events = 0;
    m_pollfds[hint].revents = 0;

    EventHandler *handler = m_handlers[hint];
    m_handlers[hint] = NULL;

    return handler;
}

void Reactor::handleEvents()
{
    int res = poll(m_pollfds, MAX_FDS, -1);

    if  (res < 0)
    {
        throw Exception(errno);
    }

    unsigned int handledEvents = 0;
    for (unsigned int i = 0; i < MAX_FDS && handledEvents < res; ++i)
    {
        if (m_pollfds[i].revents != 0)
        {
            assert(m_handlers[i] != NULL);

            short revents = m_pollfds[i].revents;
            m_pollfds[i].revents = 0;

            EventHandler *handler = m_handlers[i];

            if ((revents & POLLERR) || (revents & POLLNVAL))
            {
                handler->handleError(i);
            }
            else if (revents & POLLHUP)
            {
                handler->handleClose(i);
            }
            else if (revents & POLLIN)
            {
                handler->handleInput(i);
            }
            else if (revents & POLLOUT)
            {
                handler->handleOutput(i);
            }

            handledEvents++;
        }
    }
}

} // namespace followermaze
