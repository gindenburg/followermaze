/*
 * This file contains implementation of Connection based on Berkley sockets API.
 * This is a very simple implementation :-)
 */

#include "connection.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

namespace followermaze
{

Connection::Connection(int portno, bool async)
{
    // Create socket (we assume TCP/IP with IPv4 for simplicity)
    int type = async ? (SOCK_STREAM | SOCK_NONBLOCK) : SOCK_STREAM;
    m_handle = socket(AF_INET, type, 0);
    if (m_handle < 0)
    {
        throw Exception(errno);
    }

    // Make socket reuse the address to enable server restart without waiting.
    int so_reuseaddr = 1;
    if (setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr)) < 0)
    {
        close(m_handle);
        throw Exception(errno);
    }

    // Initialize socket address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portno);

    // Now bind the host address using bind() call.
    if (0 != bind(m_handle, (struct sockaddr *) &serverAddr, sizeof(serverAddr)))
    {
        close(m_handle);
        throw Exception(errno);
    }

    if (0 != listen(m_handle, 5))
    {
        close(m_handle);
        throw Exception(errno);
    }
}

Connection::Connection()
{
    m_handle = -1;
}

Connection::~Connection()
{
    if (m_handle >= 0)
    {
        close(m_handle);
    }
}

Connection* Connection::accept(bool async)
{
    Connection* clientConnection = new Connection();

    struct sockaddr clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int flags = async ? SOCK_NONBLOCK : 0;
    int sockfd = ::accept4(m_handle, (struct sockaddr *)&clientAddr, &clientAddrLen, flags);
    if (sockfd < 0)
    {
        delete clientConnection;
        throw Exception(errno);
    }

    clientConnection->m_handle = sockfd;

    return clientConnection;
}

const char *Connection::receive()
{
    ssize_t bytesRecieved = 0;

    bytesRecieved = recv(m_handle, m_buffer, 1023, 0);

    if (bytesRecieved == 0)
    {
        // Client closed the connection.
        throw Exception(Exception::ErrClientDisconnect);
    }
    else if (bytesRecieved < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK))
    {
        // Error.
        throw Exception(errno);
    }

    m_buffer[bytesRecieved] = 0; // zterminate

    return m_buffer;
}

void Connection::send(const string &message)
{
    int flags = MSG_NOSIGNAL;
    if (::send(m_handle, message.c_str(), message.length(), flags) < 0)
    {
        // Client closed the connection or some error has happened.
        throw Exception(errno == EPIPE ? Exception::ErrClientDisconnect : errno);
    }
}

Handle Connection::getHandle() const
{
    return m_handle;
}

} // namespace followermaze
