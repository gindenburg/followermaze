#include <assert.h>
#include "server.h"
#include "logger.h"

namespace followermaze
{

Server::~Server()
{
}

void Server::serve()
{
    try
    {
        initReactor();

        for (;;)
        {
            m_reactor.handleEvents();
        }

        assert(false);
    }
    catch (Reactor::Exception e)
    {
        if (e.getErr() == Reactor::Exception::ErrStop)
        {
            Logger::getInstance().message("Reactor stopped.");
        }
    }
}

} // namespace followermaze
