#include <iostream>
#include <stdlib.h>
#include "server.h"
#include "acceptor.h"
#include "protocol.h"
#include "engine.h"
#include "logger.h"

using namespace followermaze;

// SimpleServer is a simple single threaded Reactor based server with CLI.
class SimpleServer : public Server
{
public:
    class Config
    {
        friend class SimpleServer;

    protected:
        static const int ADMIN_PORT = 9999;
        static const int DEFAULT_EVENT_PORT = 9090;
        static const int DEFAULT_USER_PORT = 9099;

    public:
        Config(int argc, char *argv[]) :
            m_valid(false),
            m_stop(false),
            m_help(false),
            m_adminPort(ADMIN_PORT),
            m_eventPort(DEFAULT_EVENT_PORT),
            m_userPort(DEFAULT_USER_PORT)
        {
            if (argc > 3)
            {
                m_help = true;
                return;
            }

            if (argc == 2)
            {
                // We've got a command
                if (string("-h").compare(argv[1]) == 0 ||
                    string("--help").compare(argv[1]) == 0)
                {
                    m_help = true;
                }
                else if (string("stop").compare(argv[1]) == 0)
                {
                    m_stop = true;
                }
                else
                {
                    Logger::getInstance().error("Invalid command: ", argv[1]);
                    return;
                }
            }
            else if (argc == 3)
            {
                // We've got options
                m_eventPort = protocol::Parser::parseLong(argv[1]);
                if (m_eventPort == protocol::Parser::INVALID_LONG || m_eventPort <= 1024 || m_eventPort > 65535)
                {
                    Logger::getInstance().error("Invalid event_source_port: ", argv[1]);
                    return;
                }

                m_userPort = protocol::Parser::parseLong(argv[2]);
                if (m_userPort == protocol::Parser::INVALID_LONG || m_userPort <= 1024 || m_userPort > 65535)
                {
                    Logger::getInstance().error("Invalid user_client_port: ", argv[2]);
                    return;
                }
            }

            m_valid = true;
        }

    public:
        bool m_stop;
        bool m_help;
        bool m_valid;

    protected:
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
    protocol::Engine m_engine;
    ClientFactory<Admin> m_adminFactory;
    protocol::EngineDrivenClientFactory<protocol::EventSource> m_eventSourceFactory;
    protocol::EngineDrivenClientFactory<protocol::UserClient> m_userClientFactory;
};

int main(int argc, char *argv[])
{
    SimpleServer::Config config(argc, argv);

    if (config.m_help)
    {
        static const char *usage = "followermaze is a server which expects event source and user clients on given ports.\n" \
                                   "Port 9999 is reserved.\n" \
                                   "Usage(1): followermaze -h|--help\n" \
                                   "Usage(2): followermaze [event_source_port user_client_port]\n" \
                                   "Usage(3): followermaze stop\n" \
                                   "Options:\n" \
                                   "  -h, --help - print usage\n" \
                                   "  event_source_port - port to expect the event source on. Default 9090.\n" \
                                   "  user_client_port - port to expect the user clients on. Default 9099.\n" \
                                   "Commands:\n"
                                   "  stop - stops the server\n";
        cout << usage;
        return config.m_valid ? 0 : 1;
    }

    if (!config.m_valid)
    {
        return 1;
    }

    if (config.m_stop)
    {
        static const char *STOP_COMMAND = "echo stop | nc localhost 9999";
        system(STOP_COMMAND);
        return 0;
    }

    try
    {
        SimpleServer server(config);
        server.serve();

        Logger::getInstance().info("Server shutting down.");
    }
    catch (BaseException &e)
    {
        Logger::getInstance().error("Server failed. Error: ", e.what(), e.getErr());
        return 1;
    }

    return 0;
}
