#include <iostream>  
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")  


typedef struct
{
    WSAOVERLAPPED m_overlapped;
    SOCKET m_socket;
    WSABUF m_wsa_buf;
    char m_buffer[1024] = { 0 };
    DWORD m_bytes_sent;
    DWORD m_bytes_to_sent;
} PER_IO_DATA, *LPPER_IO_DATA;


static DWORD WINAPI ServerWorkerThread(LPVOID lpParameter)
{
    HANDLE h_completion_port = (HANDLE)lpParameter;
    DWORD bytes_sent = 0;
    ULONG completion_key;
    LPPER_IO_DATA per_io_data;

    while (GetQueuedCompletionStatus(h_completion_port, &bytes_sent, (PULONG_PTR)&completion_key, (LPOVERLAPPED*)&per_io_data, INFINITE))
    {
        if (!per_io_data)
            continue;

        if (bytes_sent == 0)
        {
            std::cout << "Client disconnected!\r\n\r\n";
        }
        else
        {
            per_io_data->m_bytes_sent += bytes_sent;
            if (per_io_data->m_bytes_sent < per_io_data->m_bytes_to_sent)
            {
                per_io_data->m_wsa_buf.buf = &(per_io_data->m_buffer[per_io_data->m_bytes_sent]);
                per_io_data->m_wsa_buf.len = (per_io_data->m_bytes_to_sent - per_io_data->m_bytes_sent);
            }
            else
            {
                per_io_data->m_wsa_buf.buf = per_io_data->m_buffer;
                per_io_data->m_wsa_buf.len = strlen(per_io_data->m_buffer);
                per_io_data->m_bytes_sent = 0;
                per_io_data->m_bytes_to_sent = per_io_data->m_wsa_buf.len;
            }

            if (WSASend(per_io_data->m_socket, &(per_io_data->m_wsa_buf), 1, &bytes_sent, 0, &(per_io_data->m_overlapped), NULL) == 0)
                continue;

            if (WSAGetLastError() == WSA_IO_PENDING)
                continue;
        }

        closesocket(per_io_data->m_socket);
        delete per_io_data;
    }

    return 0;
}

int main()
{
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
        return 0;

    HANDLE h_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!h_completion_port)
        return 0;

    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    for (DWORD i = 0; i < system_info.dwNumberOfProcessors; ++i)
    {
        HANDLE h_thread = CreateThread(NULL, 0, ServerWorkerThread, h_completion_port, 0, NULL);
        CloseHandle(h_thread);
    }

    SOCKET listen_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (listen_socket == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN server;
    ZeroMemory(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(listen_socket, (SOCKADDR*)(&server), sizeof(server)) != 0)
        return 0;

    if (listen(listen_socket, 1) != 0)
        return 0;

    std::cout << "Waiting for incoming connection...\r\n";

    SOCKET accept_socket;
    do
    {
        sockaddr_in sa_client;
        int client_size = sizeof(sa_client);
        accept_socket = WSAAccept(listen_socket, (SOCKADDR*)&sa_client, &client_size, NULL, NULL);
    } while (accept_socket == INVALID_SOCKET);

    std::cout << "Client connected!\r\n\r\n";

    CreateIoCompletionPort((HANDLE)accept_socket, h_completion_port, 0, 0);

    LPPER_IO_DATA p_per_io_data = new PER_IO_DATA;
    ZeroMemory(p_per_io_data, sizeof(PER_IO_DATA));

    strcpy_s(p_per_io_data->m_buffer, "Welcome to the server!\r\n");

    p_per_io_data->m_overlapped.hEvent = WSACreateEvent();
    p_per_io_data->m_socket = accept_socket;
    p_per_io_data->m_wsa_buf.buf = p_per_io_data->m_buffer;
    p_per_io_data->m_wsa_buf.len = strlen(p_per_io_data->m_buffer);
    p_per_io_data->m_bytes_to_sent = p_per_io_data->m_wsa_buf.len;

    DWORD dw_num_sent;
    if (WSASend(accept_socket, &(p_per_io_data->m_wsa_buf), 1, &dw_num_sent, 0, &(p_per_io_data->m_overlapped), NULL) == SOCKET_ERROR)
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

    shutdown(accept_socket, SD_BOTH);
    closesocket(accept_socket);

    WSACleanup();
    return 0;
}