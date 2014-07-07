#include "server.h"
#include "acceptor.h"
#include "protocol.h"
#include "logger.h"

using namespace followermaze;

class SimpleServer : public Server
{
public:
    class Config
    {
    public:
        int m_eventPort;
        int m_userPort;
        int m_adminPort;
    };

    SimpleServer(const Config& config) : m_config(config)
    {
    }

    Protocol::UserClient *createClient(auto_ptr<Connection> conn, Reactor &reactor)
    {
        return new Protocol::UserClient(conn, reactor, m_engine);
    }

    virtual void initReactor()
    {
        auto_ptr<EventHandler> adminAcceptor(new Acceptor<Admin>(m_config.m_adminPort, m_reactor, m_engine));
        m_reactor.addHandler(adminAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for admins on port ", m_config.m_adminPort);

        auto_ptr<EventHandler> eventAcceptor(new Acceptor<Protocol::EventSource>(m_config.m_eventPort, m_reactor, m_engine));
        m_reactor.addHandler(eventAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for events on port ", m_config.m_eventPort);

        auto_ptr<EventHandler> userAcceptor(new Acceptor<Protocol::UserClient>(m_config.m_userPort, m_reactor, m_engine));
        m_reactor.addHandler(userAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for users on port ", m_config.m_userPort);
    }

protected:
    Config  m_config;
    Engine m_engine;
};

int main()
{
    try
    {
        //Logger::getInstance().setLogLevel(Logger::LvlInfo);

        SimpleServer::Config config;
        config.m_eventPort = 9090;
        config.m_userPort = 9099;
        config.m_adminPort = 9999;

        SimpleServer server(config);
        server.serve();

        Logger::getInstance().info("Server shutting down.");
    }
    catch (BaseException e)
    {
        Logger::getInstance().error("Server failed. Error: ", e.what(), e.getErr());
    }

    return 0;
}
