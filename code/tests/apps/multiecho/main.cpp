#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <climits>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <fcntl.h>
#include <assert.h>
#include "client.h"
#include "server.h"
#include "acceptor.h"
#include "logger.h"


using namespace std;

namespace followermaze
{

class Thread
{
public:
    class Exception : public BaseException
    {
    public:
        Exception(int err = BaseException::ErrGeneric) : BaseException(err) {}
        virtual const char* what() const throw() { return "Thread::Exception"; }
    };

    class Watchdog : public EventHandler
    {
    public:
        class Exception : public BaseException
        {
        public:
            enum
            {
                ErrStopSignal = BaseException::ErrGeneric + 1
            };

            Exception(int err = BaseException::ErrGeneric) : BaseException(err) {}
            virtual const char* what() const throw() { return "Watchdog::Exception"; }
        };

    public:
        Watchdog(Thread& thread) : m_thread(thread)
        {
        }

        virtual Handle getHandle()
        {
            return m_thread.m_pfd[0];
        }

        virtual void handleInput()
        {
            for (;;)
            {
                char ch;
                if (read(m_thread.m_pfd[0], &ch, 1) == -1)
                {
                    if (errno == EAGAIN)
                    {
                        throw Exception(Exception::ErrStopSignal);
                    }

                    throw Exception(errno);
                }
            }
        }

    private:
        Thread  &m_thread;
    };

    Thread()
    {
        memset(&m_pfd, -1, sizeof(m_pfd));

        if (pipe2(m_pfd, O_NONBLOCK) == -1)
        {
            throw Exception(errno);
        }

    }

    virtual ~Thread()
    {
        if (m_pthread != 0)
        {
            try
            {
                stop(false);
            }
            catch (Exception e)
            {
                Logger::getInstance().message("Exception in Thread destructor. Ignore.");
            }
        }

        close(m_pfd[0]);
        close(m_pfd[1]);
    }

    void start()
    {
        int err = pthread_create(&m_pthread, NULL, threadFunction, this);
        if (err)
        {
            throw Exception(err);
        }
    }

    void stop(bool wait)
    {
        if (m_pthread == 0)
        {
            throw Exception();
        }

        // send signal to the thread
        if (write(m_pfd[1], "x", 1) == -1 && errno != EAGAIN)
        {
            throw Exception(errno);
        }

        if (wait)
        {
            join();
        }
    }

    void join()
    {
        if (m_pthread == 0)
        {
            throw Exception();
        }

        int err = pthread_join(m_pthread, NULL);
        if (err != 0 && err != EDEADLK)
        {
            throw Exception(err);
        }

        m_pthread = 0;
    }

    static void* threadFunction(void *arg);

protected:
    pthread_t m_pthread;
    int       m_pfd[2];

protected:
    virtual void run() = 0;

private:
    Thread(const Thread&);
    Thread& operator=(const Thread&);
};

void* Thread::threadFunction(void *arg)
{
    Thread* self = (Thread*)arg;
    if (self)
    {
        self->run();
    }
}

class Protocol
{
public:
    static int parseUserId(const string &msg)
    {
        if (!isWellFormed(msg))
        {
            return INVALID_USER_ID;
        }

        int id = strtol(msg.c_str(), NULL, 10);

        if (id == 0 || id == LONG_MAX || id == LONG_MIN)
        {
            return INVALID_USER_ID;
        }

        return id;
    }

    static bool isWellFormed(const string &msg)
    {
        if (msg.length() < 2)
        {
            return false;
        }

        if (msg.compare(msg.length() - 2, 2, crlf) != 0)
        {
            return false;
        }

        return true;
    }

    static const char* crlf;
    static const int INVALID_USER_ID;
};

const char* Protocol::crlf = "\r\n";
const int Protocol::INVALID_USER_ID = -1;

}

using namespace followermaze;

class EchoClient : public Client
{
public:
    EchoClient(auto_ptr<Connection> conn, Reactor &reactor) : Client(conn, reactor) {}

protected:
    virtual void doHandleInput(int hint)
    {
        m_message += m_conn->receive();
        m_reactor.resetHandler(hint, Reactor::EvntWrite);
    }

    virtual void doHandleOutput(int hint)
    {
        string message("echo says: ");
        message += m_message;
        m_conn->send(message);
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
        int m_eventPort;
        int m_userPort;
        int m_adminPort;
    };

    EchoServer(const Config& config) : m_config(config)
    {
    }

    virtual void initReactor()
    {
        auto_ptr<EventHandler> adminAcceptor(new Acceptor<Admin>(m_config.m_adminPort, m_reactor));
        m_reactor.addHandler(adminAcceptor, Reactor::EvntAccept);
        Logger::getInstance().message("Listening for admins on port ", m_config.m_adminPort);

        auto_ptr<EventHandler> userAcceptor(new Acceptor<EchoClient>(m_config.m_userPort, m_reactor));
        m_reactor.addHandler(userAcceptor, Reactor::EvntAccept);
        Logger::getInstance().message("Listening for users on port ", m_config.m_userPort);
    }

protected:
    Config  m_config;
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

        Logger::getInstance().message("Server shutting down.");
    }
    catch (BaseException e)
    {
        string message(e.what());
        message += ": ";
        Logger::getInstance().error(message, e.getErr());
        Logger::getInstance().message("Server failed. Shutting down.");
    }

    return 0;
}
