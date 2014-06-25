#include <iostream>
#include <string>
#include <memory>
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <assert.h>
#include <vector>
#include <set>
#include "connection.h"

using namespace std;

namespace followermaze
{

class Logger
{
public:
    Logger() : m_log(true) {}

    void message(const string &msg)
    {
        cout << "MSG: " << msg.c_str() << endl << flush;
    }

    void message(const string &msg, int number)
    {
        cout << "MSG: " << msg.c_str() << number << "." << endl << flush;
    }

    void error(const string &msg, int err)
    {
        cout << "ERR: " << msg.c_str() << "(" << err << ")." << endl << flush;
    }

    void shutUp()
    {
        m_log = true;
    }

    void speak()
    {
        m_log = false;
    }

    static Logger& getInstance()
    {
        return m_logger;
    }

protected:
    static Logger m_logger;

    bool m_log;
};

Logger Logger::m_logger;

class EventHandler
{
public:
    virtual ~EventHandler() {}

    virtual Handle getHandle() = 0;

    virtual void handleInput(int hint)
    {
    }

    virtual void handleOutput(int hint)
    {
    }

    virtual void handleClose(int hint)
    {
    }

    virtual void handleError(int hint)
    {
    }
};

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

        Exception(int err = BaseException::ErrGeneric) : BaseException(err) {}
        virtual const char* what() const throw() { return "Reactor::Exception"; }
    };

    typedef unsigned int EventType;
    enum
    {
        EvntAccept = 0x01,
        EvntRead = 0x01,
        EvntWrite = 0x02,
        EvntAll = 0x03
    };

    Reactor()
    {
        for (unsigned int i = 0; i < MAX_FDS; ++i)
        {
            m_pollfds[i].fd = -1;
            m_pollfds[i].events = 0;
            m_pollfds[i].revents = 0;
            m_handlers[i] = NULL;
        }
    }

    ~Reactor()
    {
        for (unsigned int i = 0; i < MAX_FDS; ++i)
        {
            delete m_handlers[i];
        }
    }

    void addHandler(auto_ptr<EventHandler> handler, EventType event)
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

    void resetHandler(int hint, EventType event)
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

    void detouchHandler(int hint)
    {
        if (hint < 0 || hint >= MAX_FDS)
        {
            throw Exception();
        }

        m_pollfds[hint].fd = -1;
        m_pollfds[hint].events = 0;
        m_pollfds[hint].revents = 0;
        m_handlers[hint] = NULL;
    }

    void handleEvents()
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

protected:
    static const unsigned int MAX_FDS = 128;

    struct pollfd m_pollfds[MAX_FDS];
    EventHandler *m_handlers[MAX_FDS];
};

class Client : public EventHandler
{
protected:
    virtual ~Client() {}

public:
    Client(auto_ptr<Connection> conn, Reactor &reactor) : m_conn(conn), m_reactor(reactor)
    {
        assert(m_conn.get() != NULL);
    }

    virtual Handle getHandle()
    {
        return m_conn->getHandle();
    }

    virtual void handleClose(int hint)
    {
        disconnect(hint);
    }

    virtual void handleError(int hint)
    {
        Logger::getInstance().message("Client error.");
        disconnect(hint);
    }

protected:
    void disconnect(int hint)
    {
        m_reactor.detouchHandler(hint);
        delete this;
        Logger::getInstance().message("Client disconnected.");
    }

protected:
    auto_ptr<Connection> m_conn;
    Reactor &m_reactor;
};

class AdminClient : public Client
{
public:
    AdminClient(auto_ptr<Connection> conn, Reactor &reactor) : Client(conn, reactor) {}

    virtual void handleInput(int hint)
    {
        try
        {
            string command = m_conn->receive();

            if (command.compare(0, 4, "stop") == 0)
            {
                Logger::getInstance().message("Got stop command.");
                throw Reactor::Exception(Reactor::Exception::ErrStop);
            }
        }
        catch (Connection::Exception e)
        {
            if (e.getErr() == Connection::Exception::ErrClientDisconnect)
            {
                disconnect(hint);
            }
            else
            {
                throw e;
            }
        }
    }

protected:
    virtual ~AdminClient() {}
};

class UserClient : public Client
{
public:
    UserClient(auto_ptr<Connection> conn, Reactor &reactor) : Client(conn, reactor) {}

    virtual void handleInput(int hint)
    {
        try
        {
            m_message += m_conn->receive();
            m_reactor.resetHandler(hint, Reactor::EvntWrite);
        }
        catch (Connection::Exception e)
        {
            if (e.getErr() == Connection::Exception::ErrClientDisconnect)
            {
                disconnect(hint);
            }
            else
            {
                throw e;
            }
        }
    }

    virtual void handleOutput(int hint)
    {
        try
        {
            string message("echo goes: ");
            message += m_message;
            m_conn->send(message);
            m_reactor.resetHandler(hint, Reactor::EvntRead);
            m_message.clear();
        }
        catch (Connection::Exception e)
        {
            if (e.getErr() == Connection::Exception::ErrClientDisconnect)
            {
                disconnect(hint);
            }
            else
            {
                throw e;
            }
        }
    }

protected:
    virtual ~UserClient() {}

protected:
    string m_message;
};

template < class ClientType >
class Acceptor : public EventHandler
{
public:
    Acceptor(int port, Reactor &reactor) : m_conn(port), m_reactor(reactor)
    {
    }

    virtual Handle getHandle()
    {
        return m_conn.getHandle();
    }

    virtual void handleInput(int /*hint*/)
    {
        auto_ptr<Connection> clientConn(m_conn.accept(true));
        auto_ptr<EventHandler> client(new ClientType(clientConn, m_reactor));

        m_reactor.addHandler(client, Reactor::EvntRead);

        client.release();

        Logger::getInstance().message("Client connected.");
    }

protected:
    virtual ~Acceptor() {}

protected:
    Connection m_conn;
    Reactor &m_reactor;
};

class Server
{
public:
    class Exception : public BaseException
    {
    public:
        Exception(int err = BaseException::ErrGeneric) : BaseException(err) {}
        virtual const char* what() const throw() { return "Server::Exception"; }
    };

    class Config
    {
    public:
        int m_userPort;
        int m_adminPort;
    };

    Server(const Config& config) : m_config(config)
    {
    }

    void serve()
    {
        try
        {
            auto_ptr<EventHandler> userAcceptor(new Acceptor<UserClient>(m_config.m_userPort, m_reactor));
            m_reactor.addHandler(userAcceptor, Reactor::EvntAccept);
            Logger::getInstance().message("Listening for users on port ", m_config.m_userPort);

            auto_ptr<EventHandler> adminAcceptor(new Acceptor<AdminClient>(m_config.m_adminPort, m_reactor));
            m_reactor.addHandler(adminAcceptor, Reactor::EvntAccept);
            Logger::getInstance().message("Listening for admins on port ", m_config.m_adminPort);

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
                Logger::getInstance().message("Reactor stopped. Shutting down.");
            }
        }
    }

protected:
    Reactor m_reactor;
    Config  m_config;
};

}

using namespace followermaze;

int main()
{
    try
    {
        Server::Config config;
        config.m_userPort = 9090;
        config.m_adminPort = 9099;

        Server server(config);
        server.serve();
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
