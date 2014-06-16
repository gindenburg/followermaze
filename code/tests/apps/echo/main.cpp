#include <iostream>
#include <string>
#include <memory>
#include "connection.h"

using namespace std;

int main()
{
    try
    {
        Connection eventSourceListener(9090);
        auto_ptr<Connection> eventSource(eventSourceListener.accept());

        while (eventSource->isAlive())
        {
            string message = eventSource->receive();
            cout << message.c_str() << endl;
            eventSource->send("Got a message: ");
            eventSource->send(message);
        }
    }
    catch (Connection::Exception e)
    {
        if (e.getErr() == Connection::Exception::ErrClientDisconnect)
        {
            cout << "Client disconnected. Shutting down." << endl;
        }
        else
        {
            cout << e.what() << ": " << e.getErr() << ". Shutting down." << endl;
        }
    }

    return 0;
}
