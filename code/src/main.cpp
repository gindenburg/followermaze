#include <iostream>
#include "server.h"
#include "acceptor.h"
#include "protocol.h"
#include "engine.h"
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

    SimpleServer(const Config& config) :
        m_config(config),
        m_eventSourceFactory(m_engine),
        m_userClientFactory(m_engine)
    {
    }

    virtual void initReactor()
    {
        auto_ptr<EventHandler> adminAcceptor(new Acceptor(m_config.m_adminPort, m_reactor, m_adminFactory));
        m_reactor.addHandler(adminAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for admins on port ", m_config.m_adminPort);

        auto_ptr<EventHandler> eventAcceptor(new Acceptor(m_config.m_eventPort, m_reactor, m_eventSourceFactory));
        m_reactor.addHandler(eventAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for events on port ", m_config.m_eventPort);

        auto_ptr<EventHandler> userAcceptor(new Acceptor(m_config.m_userPort, m_reactor, m_userClientFactory));
        m_reactor.addHandler(userAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for users on port ", m_config.m_userPort);
    }

protected:
    Config  m_config;
    ClientFactory<Admin> m_adminFactory;
    protocol::Engine m_engine;
    protocol::ClientFactory<protocol::EventSource> m_eventSourceFactory;
    protocol::ClientFactory<protocol::UserClient> m_userClientFactory;
};

int main()
{
    SimpleServer::Config config;
    config.m_eventPort = 9090;
    config.m_userPort = 9099;
    config.m_adminPort = 9999;

    try
    {
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
