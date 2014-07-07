#include "test.h" // Brings in the UnitTest++ framework
#include "protocol.h"

using namespace std;
using namespace followermaze;

TEST(ParseMessage)
{
    string message;
    size_t start = 0;

    CHECK(Protocol::Parser::findMessage("message\r\n", start, message));
    CHECK_EQUAL(message, "message");
    CHECK_EQUAL(start, string::npos);
    CHECK(!Protocol::Parser::findMessage("message\r\n", start, message));
    CHECK(!Protocol::Parser::findMessage("message\r\n", start, message));
}

TEST(ParseEmptyMessage)
{
    string message;
    size_t start = 0;

    CHECK(Protocol::Parser::findMessage("\r\n", start, message));
    CHECK_EQUAL(message, "");
    CHECK_EQUAL(start, string::npos);
}

TEST(ParseInvalidMessage)
{
    string message;
    size_t start = 0;

    CHECK(!Protocol::Parser::findMessage("message", start, message));
    CHECK_EQUAL(message, "");
    CHECK_EQUAL(start, 0);
}

TEST(ParseMultiMessage)
{
    string in = "message1\r\nmessage2\r\nmessage3\r\nincomplete";
    string message;
    size_t start = 0;

    CHECK(Protocol::Parser::findMessage(in, start, message));
    CHECK_EQUAL(message, "message1");
    CHECK_EQUAL(start, 10);
    CHECK(Protocol::Parser::findMessage(in, start, message));
    CHECK_EQUAL(message, "message2");
    CHECK_EQUAL(start, 20);
    CHECK(Protocol::Parser::findMessage(in, start, message));
    CHECK_EQUAL(message, "message3");
    CHECK_EQUAL(start, 30);
    CHECK(!Protocol::Parser::findMessage(in, start, message));
    CHECK_EQUAL(start, 30);
    CHECK(!Protocol::Parser::findMessage(in, start, message));
}

TEST(ParseLong)
{
    string message = "1234";
    CHECK_EQUAL(Protocol::Parser::parseLong(message), 1234l);

    message = "0";
    CHECK_EQUAL(Protocol::Parser::parseLong(message), Protocol::Parser::INVALID_LONG);

    message = "alk2lk";
    CHECK_EQUAL(Protocol::Parser::parseLong(message), Protocol::Parser::INVALID_LONG);

    message = "999999999999999999999999999999999999999999999999999999999999999";
    CHECK_EQUAL(Protocol::Parser::parseLong(message), Protocol::Parser::INVALID_LONG);

    message = "-999999999999999999999999999999999999999999999999999999999999999";
    CHECK_EQUAL(Protocol::Parser::parseLong(message), Protocol::Parser::INVALID_LONG);
}

TEST(ParseEvent)
{
    Protocol::Event event;
    event.m_payload = "123456|F|789|12345";
    Protocol::Parser::parseEvent(event);
    CHECK(Protocol::Parser::isValidEvent(event));
    CHECK_EQUAL(event.m_seqnum, 123456);
    CHECK_EQUAL(event.m_type, Protocol::Parser::TYPE_FOLLOW);
    CHECK_EQUAL(event.m_fromUserId, 789);
    CHECK_EQUAL(event.m_toUserId, 12345);

    event.m_payload = "9876|U|543|210";
    Protocol::Parser::parseEvent(event);
    CHECK(Protocol::Parser::isValidEvent(event));
    CHECK_EQUAL(event.m_seqnum, 9876);
    CHECK_EQUAL(event.m_type, Protocol::Parser::TYPE_UNFOLLOW);
    CHECK_EQUAL(event.m_fromUserId, 543);
    CHECK_EQUAL(event.m_toUserId, 210);

    event.m_payload = "123456789|B";
    Protocol::Parser::parseEvent(event);
    CHECK(Protocol::Parser::isValidEvent(event));
    CHECK_EQUAL(event.m_seqnum, 123456789);
    CHECK_EQUAL(event.m_type, Protocol::Parser::TYPE_BROADCAST);
    CHECK_EQUAL(event.m_fromUserId, Protocol::Parser::INVALID_LONG);
    CHECK_EQUAL(event.m_toUserId, Protocol::Parser::INVALID_LONG);

    event.m_payload = "9876|P|543|210";
    Protocol::Parser::parseEvent(event);
    CHECK(Protocol::Parser::isValidEvent(event));
    CHECK_EQUAL(event.m_seqnum, 9876);
    CHECK_EQUAL(event.m_type, Protocol::Parser::TYPE_PRIVATE);
    CHECK_EQUAL(event.m_fromUserId, 543);
    CHECK_EQUAL(event.m_toUserId, 210);

    event.m_payload = "123456|S|789";
    Protocol::Parser::parseEvent(event);
    CHECK(Protocol::Parser::isValidEvent(event));
    CHECK_EQUAL(event.m_seqnum, 123456);
    CHECK_EQUAL(event.m_type, Protocol::Parser::TYPE_STATUSUPDATE);
    CHECK_EQUAL(event.m_fromUserId, 789);
    CHECK_EQUAL(event.m_toUserId, Protocol::Parser::INVALID_LONG);
}

TEST(ParseInvalidEvent)
{
    Protocol::Event event;

    event.m_payload = "";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "err|B";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|err";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|F";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|F|err|456";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|F|456|err";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|U";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|U|err|456";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|U|456|err";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|B|456";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|B|456|789";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|P";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|P|err|456";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|P|456|err";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|S";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));

    event.m_payload = "123|S|456|789";
    Protocol::Parser::parseEvent(event);
    CHECK(!Protocol::Parser::isValidEvent(event));
}

TEST(EncodeMessage)
{
    string message;
    Protocol::Parser::encodeMessage("message", message);
    CHECK_EQUAL("message\n", message);
}

TEST(EventQueue)
{
    Protocol::EventQueue eventQueue;

    Protocol::Event event0;
    event0.m_seqnum = 3;
    eventQueue.push(&event0);

    Protocol::Event event1;
    event1.m_seqnum = 1;
    eventQueue.push(&event1);

    Protocol::Event event2;
    event2.m_seqnum = 0;
    eventQueue.push(&event2);

    Protocol::Event event3;
    event3.m_seqnum = 2;
    eventQueue.push(&event3);

    int i = 0;
    while (!eventQueue.empty())
    {
        CHECK_EQUAL(eventQueue.top()->m_seqnum, i);
        eventQueue.pop();
        i++;
    }
}
