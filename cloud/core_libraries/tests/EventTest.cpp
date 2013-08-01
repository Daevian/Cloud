#include "StdAfx.h"
#include <cfixcc.h>

#include <stdio.h>
//#include <string>
#include "../utilities/CloudEvent.h"

class EventTest : public cfixcc::TestFixture
{
    class Server
    {
    public:
        Cloud::Event<Server> MessageSent;

        void OnSendMessage(CLchar* p_msg)
        {
            MessageSent(this, p_msg);
        }
    };

    class ClientPrivate
    {
    public:
        ClientPrivate(Server* server)
            :m_message("")
        {
            server->MessageSent += new Cloud::Delegate<ClientPrivate, Server>(this, &ClientPrivate::MessageSendHandler);
        }

        CLchar* m_message;

    private:
        void MessageSendHandler(const Server* sender, void* parameter)
        {
            m_message = (CLchar*)parameter;
        }
    };

    class ClientPublic
    {
    public:
        ClientPublic()
            :m_message("")
        {
        }
        
        void MessageSendHandler(const Server* sender, void* parameter)
        {
            m_message = (CLchar*)parameter;
        }
        
        CLchar* m_message;
        
    };

public:
    void NoSubscribersTest()
    {
        Server server;
        server.OnSendMessage("Hello!");
    }

    void PrivateSubscribersTest()
    {
        Server server;

        ClientPrivate client1(&server);
        ClientPrivate client2(&server);
        ClientPrivate client3(&server);

        server.OnSendMessage("Hello!");

        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client1.m_message));
        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client2.m_message));
        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client3.m_message));
    }

    void PublicSubscribersTest()
    {
        Server server;

        ClientPublic client1;
        ClientPublic client2;
        ClientPublic client3;

        server.MessageSent += new Cloud::Delegate<ClientPublic, Server>(&client1, &ClientPublic::MessageSendHandler);
        server.MessageSent += new Cloud::Delegate<ClientPublic, Server>(&client3, &ClientPublic::MessageSendHandler);

        server.OnSendMessage("Hello!");

        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client1.m_message));
        CFIXCC_ASSERT_EQUALS(0, strcmp("", client2.m_message));
        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client3.m_message));
    }

    void DeletedSubscribersTest()
    {
        Server server;

        ClientPublic client1;
        ClientPublic client2;
        ClientPublic client3;

        server.MessageSent += new Cloud::Delegate<ClientPublic, Server>(&client1, &ClientPublic::MessageSendHandler);
        server.MessageSent += new Cloud::Delegate<ClientPublic, Server>(&client2, &ClientPublic::MessageSendHandler);
        server.MessageSent += new Cloud::Delegate<ClientPublic, Server>(&client3, &ClientPublic::MessageSendHandler);

        server.MessageSent -= new Cloud::Delegate<ClientPublic, Server>(&client1, &ClientPublic::MessageSendHandler);

        server.OnSendMessage("Hello!");

        CFIXCC_ASSERT_EQUALS(0, strcmp("", client1.m_message));
        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client2.m_message));
        CFIXCC_ASSERT_EQUALS(0, strcmp("Hello!", client3.m_message));
    }
};

CFIXCC_BEGIN_CLASS(EventTest)
    CFIXCC_METHOD(NoSubscribersTest)
    CFIXCC_METHOD(PrivateSubscribersTest)
    CFIXCC_METHOD(PublicSubscribersTest)
    CFIXCC_METHOD(DeletedSubscribersTest)
CFIXCC_END_CLASS()