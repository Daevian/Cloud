#ifndef CLOUD_NETWORKING_SOCKET_HEADER
#define CLOUD_NETWORKING_SOCKET_HEADER

#define _WINSOCKAPI_
#include <WinSock2.h>
#include "../utilities/DefinesTypes.h"

namespace Cloud
{
    namespace Networking
    {
        class Socket
        {
        public:
            Socket();
            ~Socket();

            CLbool Init();
            void Shutdown();

            CLbool Connect(const CLulong ipAddress, const CLushort port);
            void Disconnect();

            CLint Send();

        private:
            SOCKET m_socket;
            sockaddr_in m_address;
        };
    }
}

#endif