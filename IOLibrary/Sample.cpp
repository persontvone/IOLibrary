#include "IOSocket.h"
#include <stdio.h>

#define PORT "8080"

int main()
{
	if (InitiateWinsock() == CODE_FAIL)
	{
		return WSAGetLastError();
	}

	struct addrinfo hints = MakeAdrrHints(SOCK_STREAM);
	struct addrinfo* bind_address = GenerateBindAddress(&hints, PORT);

	SOCKET listen_socket = GetSocketHandle(bind_address);
	
	if (listen_socket == CODE_FAIL)
	{
		return WSAGetLastError();
	}

	if (BindAddress(listen_socket, bind_address) == CODE_FAIL)
	{
		return WSAGetLastError();
	}

	FreeAddrStructure(bind_address);

	if (CreateConnectionListener(listen_socket, 10) == CODE_FAIL)
	{
		return WSAGetLastError();
	}

	printf("%d\n", listen_socket);

	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	SOCKET client_socket = AcceptConnection(listen_socket, (struct sockaddr*)& client_address, client_len);
	if (client_socket == CODE_FAIL)
	{
		return WSAGetLastError();
	}

	char address_buffer[100];
	if (GetConnectedAddress(address_buffer, 100, (struct sockaddr*) & client_address, client_len) != CODE_FAIL)
	{
		printf("%s\n", address_buffer);
	}

	if (TerminateWinsock() == CODE_FAIL)
	{
		return WSAGetLastError();
	}
	return 0;
}