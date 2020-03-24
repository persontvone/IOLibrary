// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "IOSocket.h"

bool InitiateWinsock(WSADATA* wsadata)
{
	if (WSAStartup(MAKEWORD(2, 2), wsadata))
	{
		return false;
	}
	return true;
}

bool TerminateWinsock()
{
	if (WSACleanup() == SOCKET_ERROR)
	{
		return false;
	}
	return true;
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

struct addrinfo GenerateBindAddress(addrinfo* hints, const char* port, const char* host)
{
	struct addrinfo* bind_address;
	if (getaddrinfo(0, port, hints, &bind_address) != 0)
	{
		exit(WSAGetLastError());
	}
	return *bind_address;
}

