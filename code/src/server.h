#ifndef SERVER_H
#define SERVER_H

#include "reactor.h"

namespace followermaze
{

class Server
{
public:
    virtual ~Server();
    virtual void serve();

protected:
    virtual void initReactor() = 0;

protected:
    Reactor m_reactor;
};

} // namespace followermaze

#endif // SERVER_H
