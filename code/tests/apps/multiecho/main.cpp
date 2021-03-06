#include "client.h"
#include "server.h"
#include "acceptor.h"
#include "logger.h"

using namespace std;
using namespace followermaze;

class EchoClient : public Client
{
public:
    EchoClient(auto_ptr<Connection> conn, Reactor &reactor) :
        Client(conn, reactor)
    {
    }

protected:
    virtual void doHandleInput(int hint)
    {
        m_message += m_connection->receive();
        m_reactor.resetHandler(hint, Reactor::EvntWrite);
    }

    virtual void doHandleOutput(int hint)
    {
        string message("echo says: ");
        message += m_message;
        m_connection->send(message);
        m_reactor.resetHandler(hint, Reactor::EvntRead);
        m_message.clear();
    }

protected:
    virtual ~EchoClient() {}

protected:
    string m_message;
};

class EchoServer : public Server
{
public:
    class Config
    {
    public:
        int m_userPort;
        int m_adminPort;
    };

    EchoServer(const Config& config) : m_config(config)
    {
    }

    virtual void initReactor()
    {
        auto_ptr<EventHandler> adminAcceptor(new Acceptor(m_config.m_adminPort, m_reactor, m_adminFactory));
        m_reactor.addHandler(adminAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for admins on port ", m_config.m_adminPort);

        auto_ptr<EventHandler> userAcceptor(new Acceptor(m_config.m_userPort, m_reactor, m_clientFactory));
        m_reactor.addHandler(userAcceptor, Reactor::EvntAccept);
        Logger::getInstance().info("Listening for users on port ", m_config.m_userPort);
    }

protected:
    Config  m_config;
    ClientFactory<Admin> m_adminFactory;
    ClientFactory<EchoClient> m_clientFactory;
};

int main()
{
    try
    {
        EchoServer::Config config;
        config.m_userPort = 9099;
        config.m_adminPort = 9999;

        EchoServer server(config);
        server.serve();

        Logger::getInstance().info("Server shutting down.");
    }
    catch (BaseException e)
    {
        Logger::getInstance().error("Server failed. Error: ", e.what(), e.getErr());
    }

    return 0;
}
