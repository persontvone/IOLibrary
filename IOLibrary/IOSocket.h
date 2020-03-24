#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

bool InitiateWinsock(WSADATA* wsadata);
bool TerminateWinsock();

/*
 *  MakeAddrHints - Returns a structure that serves as a hint for generating a subsequent address.
 *  @param [in] socktype - Default socket types. Can be SOCK_STREAM or SOCK_DGRAM, etc.
 *  @param [in] family - Default address family. Can be AF_INET / AF_INET6. Default value AF_INET(IPv4).
 *  @param [in] flag - Flags used in "hints" argument to getaddrinfo(). Default value AI_PASSIVE, 
        this is necessary for subsequent automatic generation of the address that's suitable for further work with sockets.
 */
struct addrinfo MakeAdrrHints(int socktype, int family = AF_INET, int flags = AI_PASSIVE);

/*
    GenerateBindAddress - Returns a structure that contains eady-made necessary information for further work with sockets.
    @param [in] hints - Hints structure that serves as a hint for generating a subsequent address.
    @param [in] port - Socket port
    @param [in] host - A pointer to a NULL-terminated ANSI string that contains a host (node) name or a numeric host address string.
        To make generate address that's suitable for further work with sockets, we must pass in the first parameter as NULL.
*/
struct addrinfo GenerateBindAddress(struct addrinfo* hints, const char* port, const char* host = NULL);
