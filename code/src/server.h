/* This file declears class Server.
 */
#ifndef SERVER_H
#define SERVER_H

#include "reactor.h"

namespace followermaze
{

/* Server is an abstract class which implements the reaction (an endless
 * loop calling Reactor to handle events).
 * Subclasses should implement initialization method to seed the reaction.
 */
class Server
{
public:
    virtual ~Server();

    // Template method which calls initReactor and then starts the reaction.
    // Returns if Reactor::Exception(Reactor::ErrStop) was caught.
    virtual void serve();

protected:
    // Reactor initialization routine. Should be implemented by subclasses
    // to seed (e.g. by registering an Acceptor) the reaction.
    virtual void initReactor() = 0;

protected:
    Reactor m_reactor;
};

} // namespace followermaze

#endif // SERVER_H
