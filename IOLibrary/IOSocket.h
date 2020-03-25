#pragma once
#include "Common.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "GenericErrors.h"
#include "SocketOption.h"
#pragma comment(lib, "ws2_32.lib")

enum ErrorCode InitiateWinsock();
enum ErrorCode TerminateWinsock();

/*
 *  @param [in] socktype - Default socket types. Can be SOCK_STREAM or SOCK_DGRAM, etc.
 *  @param [in] family - Default address family. Can be AF_INET / AF_INET6. Default value AF_INET(IPv4).
 *  @param [in] flag - Flags used in "hints" argument to getaddrinfo(). Default value AI_PASSIVE, 
        this is necessary for subsequent automatic generation of the address that's suitable for further work with sockets.
    @return A structure that serves as a hint for generating a subsequent address.
 */
struct addrinfo MakeAdrrHints(int socktype, int family = AF_INET, int flags = AI_PASSIVE);

/*
    @param [in] hints - Hints structure that serves as a hint for generating a subsequent address.
    @param [in] port - Socket port
    @param [in] host - A pointer to a NULL-terminated ANSI string that contains a host (node) name or a numeric host address string.
        To make generate address that's suitable for further work with sockets, we must pass in the first parameter as NULL.
    @return A structure that contains ready-made necessary information for further work with sockets.
*/
struct addrinfo* GenerateBindAddress(struct addrinfo* phints, const char* port, const char* host = NULL);

/*
    @param [in] addr - the generated structure after calling GenerateBindAddress by which the listen socket is created.
    @return listen socket handle if no error occurs, else returns ERR_FAIL
*/
SOCKET GetSocketHandle(struct addrinfo* paddr);

/*
    Call this fucntion to bind an address with the socket.
    @param [in] socket_handle - A descriptor identifying an unbound socket.
    @param [in] paddr - sockaddr structure of the local address to assign to the bound socket.
    @return ERR_SUCCESS if address was binded to the socket handle successfully, otherwise ERR_FAIL.
*/
enum ErrorCode BindAddress(SOCKET socket_handle, struct addrinfo* paddr);
/*
    Call this function to listen for incoming connection requests.
    @param [in] listen_handle - socket handle created after calling GetSocketHandle.
    @param [in] connection_backlog - The maximum length to which the queue of pending connections can grow. Default value 5.
    @return ERR_SUCCESS if function successful, otherwise ERR_FAIL.
*/
enum ErrorCode CreateConnectionListener(SOCKET listen_handle, int connection_backlog = 5);

/*
    @param [in] socket_handle - A descriptor that identifies a socket.
    @param [in] option - SocketOption has prepared options.
    @param [in] value - boolean value for prepared option (yes/no).
    @return ERR_SUCCESS if socket option was set, otherwise ERR_FAIL.
*/
enum ErrorCode SetSocketOption(SOCKET socket_handle, enum SocketOption option, BOOLEAN value);

/*
    Call this function to get connected socket.
    @param [in] socket_handle - A descriptor that identifies a socket that has been placed in a listening state.
    @return If no error occurs, function returns a value of type SOCKET that is a descriptor for the new socket,
        otherwise ERR_FAIL.
*/
SOCKET AcceptConnection(SOCKET socket_handle, struct sockaddr* client_address, socklen_t client_len);

/*
    Call this function to get connected address.
    @param [out] buf - char buffer that will be contains connected address.
    @param [in] buf_len - length of char buffer.
    @param [in] client_address - A pointer to a socket address structure that contains the address and port number of the socket.
    @param [in] client_len - The length, in bytes, of the structure pointed to by the client_address parameter.
    return If no error occurs, function returns ERR_SUCCESS, otherwise ERR_FAIL.
*/
enum ErrorCode GetConnectedAddress(char* buf, int buf_len, struct sockaddr* client_address, socklen_t client_len);

/*
    Call this function to close socket handle.
    @param [in] socket - socket handle.
*/
enum ErrorCode CloseSocket(SOCKET socket);

/*
    The FreeAddrStructure function frees address information that the getaddrinfo function dynamically allocates in addrinfo structures.
*/
void FreeAddrStructure(struct addrinfo* paddr);
