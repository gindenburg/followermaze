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
        }
    }
    catch (Connection::Exception e)
    {
        cout << e.what() << ": " << e.getErr() << endl;
    }

    return 0;
}
