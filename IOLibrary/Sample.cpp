#include "IOSocket.h"
#include <stdio.h>
#include "IOThreading.h"
#include <stdlib.h>

#define PORT "8080"

DWORD WINAPI ServerWorkerThread(LPVOID completion_port_id);

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

	HANDLE iocp = GetCompletionPortHandle(0);
	HANDLE thread_handle = NULL;
	DWORD thread_id = 0;

	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	int thread_count = (int)system_info.dwNumberOfProcessors * 2;

	HANDLE* thread_handles = (HANDLE*)calloc(thread_count, sizeof(HANDLE));
    if (!thread_handles)
    {
        printf("Cannot allocate memory\n");
        exit(255);
    }
	for (int i = 0; i < thread_count; i++)
	{
		// Create a server worker thread and pass the completion port to the thread
		if ((thread_handle = CreateThread(NULL, 0, ServerWorkerThread, iocp, 0, &thread_id)) == NULL)
		{
			printf("CreateThread() failed with error %d\n", GetLastError());
			return 1;
		}
		else
		{
			thread_handles[i] = thread_handle;
			printf("Thread %d is OK!\n", thread_id);
		}
		CloseHandle(thread_handle);
	}

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


	free(thread_handles);

	if (TerminateWinsock() == CODE_FAIL)
	{
		return WSAGetLastError();
	}
	return 0;
}



DWORD WINAPI ServerWorkerThread(LPVOID completion_port_id)
{
    HANDLE CompletionPort = (HANDLE)completion_port_id;
    DWORD BytesTransferred;
    //LPPER_HANDLE_DATA PerHandleData;
    //LPPER_IO_OPERATION_DATA PerIoData;
    //DWORD SendBytes, RecvBytes;
    //DWORD Flags;

    //while (TRUE)
    //{
    //    if (GetQueuedCompletionStatus(CompletionPort, &BytesTransferred,
    //        (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE) == 0)
    //    {
    //        printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
    //        return 0;
    //    }
    //    else
    //    {
    //        printf("GetQueuedCompletionStatus() is OK!\n");
    //    }
    //    // First check to see if an error has occurred on the socket and if so
    //    // then close the socket and cleanup the SOCKET_INFORMATION structure
    //    // associated with the socket
    //    if (BytesTransferred == 0)
    //    {
    //        printf("Closing socket %d\n", PerHandleData->Socket);
    //        if (closesocket(PerHandleData->Socket) == SOCKET_ERROR)
    //        {
    //            printf("closesocket() failed with error %d\n", WSAGetLastError());
    //            return 0;
    //        }
    //        else
    //        {
    //            printf("closesocket() is fine!\n");
    //        }
    //        GlobalFree(PerHandleData);
    //        GlobalFree(PerIoData);
    //        continue;
    //    }
    //    // Check to see if the BytesRECV field equals zero. If this is so, then
    //    // this means a WSARecv call just completed so update the BytesRECV field
    //    // with the BytesTransferred value from the completed WSARecv() call
    //    if (PerIoData->BytesRECV == 0)
    //    {
    //        PerIoData->BytesRECV = BytesTransferred;
    //        PerIoData->BytesSEND = 0;
    //    }
    //    else
    //    {
    //        PerIoData->BytesSEND += BytesTransferred;
    //    }

    //    if (PerIoData->BytesRECV > PerIoData->BytesSEND)
    //    {
    //        // Post another WSASend() request.
    //        // Since WSASend() is not guaranteed to send all of the bytes requested,
    //        // continue posting WSASend() calls until all received bytes are sent.
    //        ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
    //        PerIoData->DataBuf.buf = PerIoData->Buffer + PerIoData->BytesSEND;
    //        PerIoData->DataBuf.len = PerIoData->BytesRECV - PerIoData->BytesSEND;
    //        if (WSASend(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &SendBytes, 0,
    //            &(PerIoData->Overlapped), NULL) == SOCKET_ERROR)
    //        {
    //            if (WSAGetLastError() != ERROR_IO_PENDING)
    //            {
    //                printf("WSASend() failed with error %d\n", WSAGetLastError());
    //                return 0;
    //            }
    //        }
    //        else
    //        {
    //            printf("WSASend() is OK!\n");
    //        }
    //    }
    //    else
    //    {
    //        PerIoData->BytesRECV = 0;
    //        // Now that there are no more bytes to send post another WSARecv() request
    //        Flags = 0;
    //        ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
    //        PerIoData->DataBuf.len = 1024;
    //        PerIoData->DataBuf.buf = PerIoData->Buffer;
    //        if (WSARecv(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags,
    //            &(PerIoData->Overlapped), NULL) == SOCKET_ERROR)
    //        {
    //            if (WSAGetLastError() != ERROR_IO_PENDING)
    //            {
    //                printf("WSARecv() failed with error %d\n", WSAGetLastError());
    //                return 0;
    //            }
    //        }
    //        else
    //        {
    //            printf("WSARecv() is OK!\n");
    //        }
    //    }
    //}
    return 1;
}