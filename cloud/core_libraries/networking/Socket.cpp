#include "Socket.h"

Cloud::Networking::Socket::Socket()
{
    ZeroMemory(&m_address, sizeof(m_address));
}

Cloud::Networking::Socket::~Socket()
{

}

CLbool Cloud::Networking::Socket::Init()
{
    WSADATA wsaData;
    CLint result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        // ASSERT : "Failed to init Client. WSAStartup() returned errorcode: " << result
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        // ASSERT : "Failed to init Client. WSAStartup() couldn't find a usable version of winsock.dll"
        WSACleanup();
        return false;
    }

    return true;
}

void Cloud::Networking::Socket::Shutdown()
{
    closesocket(m_socket);
    WSACleanup();
}

CLbool Cloud::Networking::Socket::Connect(const CLulong ipAddress, const CLushort port)
{
    // Connection setup

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socket == INVALID_SOCKET)
    {
        // ASSERT : "Failed to connect to server. socket() generated error: " << WSAGetLastError()
        return false;
    }

    ZeroMemory(&m_address, sizeof(m_address));
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = ipAddress;
    m_address.sin_port = htons(port);

    CLint result = bind(m_socket, (sockaddr*)&m_address, sizeof(m_address));

    if (result == SOCKET_ERROR)
    {
        // ASSERT : "Failed to init server. bind() generated error: " << WSAGetLastError()
        closesocket(m_socket);
        WSACleanup();
        return false;
    }

    return true;
}

void Cloud::Networking::Socket::Disconnect()
{

}

CLint Cloud::Networking::Socket::Send()
{
    int result = 0;
    //int result = sendto( m_socket, MESSAGE, LENGTH, 0,  reinterpret_cast<sockaddr*>(&m_address), sizeof(m_address));
    return result;
}