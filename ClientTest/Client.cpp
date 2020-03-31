#include <iostream>
#include <string>
#include <winsock2.h> 
#pragma comment(lib, "ws2_32.lib") 
#include "IOSocket.h"
#include "IOThreading.h""

typedef struct
{
    WSAOVERLAPPED m_overlapped;
    SOCKET m_socket;
    WSABUF m_wsa_buf;
    char m_buffer[1024];
    DWORD m_flags;
} PER_IO_DATA, *LPPER_IO_DATA;

static unsigned long __stdcall ClientWorkerThread(LPVOID lpParameter)
{
    HANDLE h_completion_port = (HANDLE)lpParameter;
    DWORD num_bytes_recv = 0;
    ULONG completion_key;
    LPPER_IO_DATA per_io_data;

    while (GetQueuedCompletionStatus(h_completion_port, &num_bytes_recv, (PULONG_PTR)&completion_key, (LPOVERLAPPED*)&per_io_data, INFINITE))
    {
        if (!per_io_data)
            continue;

        if (num_bytes_recv == 0)
        {
            std::cout << "Server disconnected!\r\n\r\n";
        }
        else
        {
            // use PerIoData->Buffer as needed...
            std::cout << std::string(per_io_data->m_buffer, num_bytes_recv);

            per_io_data->m_wsa_buf.len = sizeof(per_io_data->m_buffer);
            per_io_data->m_flags = 0;

            if (WSARecv(per_io_data->m_socket, &(per_io_data->m_wsa_buf), 1, &num_bytes_recv, &(per_io_data->m_flags), &(per_io_data->m_overlapped), NULL) == 0)
                continue;

            if (WSAGetLastError() == WSA_IO_PENDING)
                continue;
        }

        closesocket(per_io_data->m_socket);
        delete per_io_data;
    }
    return 0;
}

int main(void)
{

    if (InitiateWinsock() == CODE_FAIL)
	{
		return WSAGetLastError();
	}

    HANDLE iocp = GetCompletionPortHandle(0);
    if (!iocp)
    {
        return 0;
    }
	

    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    for (DWORD i = 0; i < system_info.dwNumberOfProcessors; ++i)
    {
        HANDLE h_thread = CreateThread(NULL, 0, ClientWorkerThread, iocp, 0, NULL);
        CloseHandle(h_thread);
    }

    struct addrinfo hints = MakeAdrrHints(SOCK_STREAM);
    SOCKET client_socket = GetOverlappedSocketHandle(&hints);
    if (client_socket == CODE_FAIL)
    {
        return WSAGetLastError();
    }

    SOCKADDR_IN sa_sock;
    sa_sock.sin_family = AF_INET;
    sa_sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa_sock.sin_port = htons(8888);

    AssociateDeviceWithCompletionPort(iocp, (HANDLE)client_socket, 0);

    if (WSAConnect(client_socket, (SOCKADDR*)(&sa_sock), sizeof(sa_sock), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
        return 0;

    PER_IO_DATA* p_per_io_data = new PER_IO_DATA;
    ZeroMemory(p_per_io_data, sizeof(PER_IO_DATA));

    p_per_io_data->m_socket = client_socket;
    p_per_io_data->m_overlapped.hEvent = WSACreateEvent();
    p_per_io_data->m_wsa_buf.buf = p_per_io_data->m_buffer;
    p_per_io_data->m_wsa_buf.len = sizeof(p_per_io_data->m_buffer);

    DWORD dw_num_recv;
    if (WSARecv(client_socket, &(p_per_io_data->m_wsa_buf), 1, &dw_num_recv, &(p_per_io_data->m_flags), &(p_per_io_data->m_overlapped), NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            delete p_per_io_data;
            return 0;
        }
    }

    while (TRUE)
    {
        Sleep(1000);
    }

    shutdown(client_socket, SD_BOTH);
    closesocket(client_socket);

    WSACleanup();
    return 0;
}