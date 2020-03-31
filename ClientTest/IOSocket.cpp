// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "IOSocket.h"
#include <stdlib.h>

enum ErrorCode InitiateWinsock()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		return CODE_FAIL;
	}
	return CODE_SUCCESS;
}

enum ErrorCode TerminateWinsock()
{
	if (WSACleanup() == SOCKET_ERROR)
	{
		return CODE_FAIL;
	}
	return CODE_SUCCESS;
}

struct addrinfo MakeAdrrHints(int socktype, int family, int flags)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	hints.ai_flags = flags;
	return hints;
}

struct addrinfo* GenerateBindAddress(struct addrinfo* phints, const char* port, const char* host)
{
	struct addrinfo* bind_address;
	if (getaddrinfo(0, port, phints, &bind_address) != 0)
	{
		exit(WSAGetLastError());
	}
	return bind_address;
}

SOCKET GetOverlappedSocketHandle(struct addrinfo* paddr)
{
	SOCKET socket = WSASocket(paddr->ai_family, paddr->ai_socktype, paddr->ai_protocol, 0, NULL, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET)
	{
		return (SOCKET)CODE_FAIL;
	}
	return socket;
}

enum ErrorCode BindAddress(SOCKET listen_handle, struct addrinfo* paddr)
{
	if (bind(listen_handle, paddr->ai_addr, paddr->ai_addrlen))
	{
		return CODE_FAIL;
	}
	return CODE_SUCCESS;
}

enum ErrorCode CreateConnectionListener(SOCKET listen_handle, int connection_backlog)
{
	if (listen(listen_handle, connection_backlog) < 0)
	{
		return CODE_FAIL;
	}
	return CODE_SUCCESS;
}

enum ErrorCode SetSocketOption(SOCKET socket_handle, enum SocketOption option, BOOLEAN value)
{
	int result = 0;
	switch (option)
	{
	case TCP_NoDelay:
		result = setsockopt(socket_handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
		break;
	case IPV6_Only:
		result = setsockopt(socket_handle, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value));
		break;
	default:
		return CODE_FAIL;
	}
	if (result != 0)
	{
		int error = WSAGetLastError();
		return CODE_FAIL;
	}
	return CODE_SUCCESS;
}

SOCKET AcceptConnection(SOCKET socket_handle, struct sockaddr* client_address, socklen_t client_len)
{
	SOCKET socket_client = WSAAccept(socket_handle, client_address, &client_len, 0, 0);
	if (socket_client == INVALID_SOCKET)
	{
		return (SOCKET)CODE_FAIL;
	}
	return socket_client;
}

//int SendData(HANDLE socket_handle, char* buffer, int buffer_length, )
//{
//	WSASend()
//}

/*
	This step is completely optional, but it is good practice to log network connections somewhere.
	getnameinfo() takes the client's address and address length. The address length is needed because
	getnameinfo() can work with both IPv4 and IPv6 addresses.We then pass in an output buffer and buffer
	length.This is the buffer that getnameinfo() writes its hostname output to.The next two arguments
	specify a second buffer and its length.getnameinfo() outputs the service name to this buffer.We don't
	care about that, so we've passed in 0 for those two parameters. Finally, we pass in the
	NI_NUMERICHOST flag, which specifies that we want to see the hostname as an IP address.
*/
enum ErrorCode GetConnectedAddress(char* buf, int buf_len, struct sockaddr* client_address, socklen_t client_len)
{
	if (getnameinfo(client_address, client_len, buf, buf_len, 0, 0, NI_NUMERICHOST) == 0)
	{
		return CODE_SUCCESS;
	}
	return CODE_FAIL;
}

enum ErrorCode CloseSocket(SOCKET socket)
{
	if (closesocket(socket) == SOCKET_ERROR)
	{
		return CODE_FAIL;
	}
	return CODE_SUCCESS;
}


void FreeAddrStructure(struct addrinfo* addr)
{
	freeaddrinfo(addr);
}