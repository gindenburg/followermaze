/* This file describes the Engine class.
 * Engine encapsulates the business logic of the followermaze application.
 * It encapsulates the state of the application (event queue and list of users)
 * and implements the logic of registering/unregistering users and processing
 * events according to the specified rules. Events are processed in the order
 * of their sequence number to ensure that the users will get them in the correct
 * order.
 */

#ifndef ENGINE_H
#define ENGINE_H

#include "protocol.h"

using namespace std;

namespace followermaze
{

using namespace Protocol;

class Engine
{
public:
    Engine();
    virtual ~Engine();

    void handleEvents(string& events);
    long registerUser(UserClient *userClient, const string& in);
    void unregisterUser(long id, UserClient *userClient);

protected:
    void handleFollow(const Event& event);
    void handleUnfollow(const Event& event);
    void handleBroadcast(const Event& event);
    void handlePrivate(const Event& event);
    void handleStatusUpdate(const Event& event);
    User* addNewUser(long id);
    void notifyUser(User *user, const string &payload);
    bool isBlankUser(const User& user);

protected:
    UserMap m_users;
    EventQueue m_events;
    long m_nextEventSeqnum;
};

} // namespace followermaze

#endif // ENGINE_H
