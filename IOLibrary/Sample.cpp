//#include "IOSocket.h"
//#include <stdio.h>
//#include "IOThreading.h"
//#include <stdlib.h>
//#include <process.h>
//#include <stdint.h>
//
//#define ACCEPT      1000 
//#define RECV_POSTED 1001
//#define SEND_POSTED 1002
//
//#define PORT "778"
//#define DATA_BUFSIZE 1024
//
//typedef struct _PER_HANDLE_DATA
//{
//    SOCKET hAcceptSocket; 
//    struct sockaddr_storage ssClientAddr;
//} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;
//
//typedef struct
//{
//    OVERLAPPED m_overlapped;
//    WSABUF m_data_buf;
//    char m_buffer[DATA_BUFSIZE];
//    uint32_t m_ui_bytes_send;
//    uint32_t m_ui_bytes_recv;
//    uint32_t m_ui_operation_type;
//    uint32_t m_ui_total_bytes;
//} PER_IO_OPERATION_DATA, *LPPER_IO_DATA;
//
//uint32_t __stdcall ServerWorkerThread(void* completion_port_id);
//
//int main()
//{
//    /*Prepare sockets */
//	if (InitiateWinsock() == CODE_FAIL)
//	{
//		return WSAGetLastError();
//	}
//	struct addrinfo hints = MakeAdrrHints(SOCK_STREAM);
//	struct addrinfo* bind_address = GenerateBindAddress(&hints, PORT);
//	SOCKET listen_socket = GetOverlappedSocketHandle(bind_address);
//	if (listen_socket == CODE_FAIL)
//	{
//		return WSAGetLastError();
//	}
//	if (BindAddress(listen_socket, bind_address) == CODE_FAIL)
//	{
//		return WSAGetLastError();
//	}
//	FreeAddrStructure(bind_address);
//
//    /* Get number of processors */
//	SYSTEM_INFO system_info;
//	GetSystemInfo(&system_info);
//	int thread_count = (int)system_info.dwNumberOfProcessors * 2;
//
//    /* Prepare thread pool for I/O completions prots */
//	HANDLE iocp = GetCompletionPortHandle(0);
//    HANDLE thread_handle = NULL;
//    uint32_t thread_id = 0;
//    HANDLE* thread_handles = (HANDLE*)calloc(thread_count, sizeof(HANDLE));
//    if (!thread_handles)
//    {
//        perror("Cannot allocate memory\n");
//        exit(255);
//    }
//    for (int i = 0; i < thread_count; i++)
//	{
//		// Create a server worker thread and pass the completion port to the thread
//		if ((thread_handle = (HANDLE)_beginthreadex(NULL, 0, &ServerWorkerThread, iocp, 0, &thread_id)) == NULL)
//		{
//			printf("CreateThread() failed with error %d\n", GetLastError());
//			return 1;
//		}
//		else
//		{
//			thread_handles[i] = thread_handle;
//			printf("Thread %d is OK!\n", thread_id);
//		}
//		CloseHandle(thread_handle);
//	}
//
//	if (CreateConnectionListener(listen_socket, 10) == CODE_FAIL)
//	{
//		return WSAGetLastError();
//	}
//    while (true)
//    {
//        PER_HANDLE_DATA* perHandleData = NULL;
//        struct sockaddr_storage client_address;
//        socklen_t client_len = sizeof(client_address);
//        SOCKET client_socket = AcceptConnection(listen_socket, (struct sockaddr*) &client_address, client_len);
//        if (client_socket == CODE_FAIL)
//        {
//            return WSAGetLastError();
//        }
//        char address_buffer[100];
//        if (GetConnectedAddress(address_buffer, 100, (struct sockaddr*) & client_address, client_len) != CODE_FAIL)
//        {
//            printf("%s\n", address_buffer);
//        }
//        perHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
//        perHandleData->hAcceptSocket = client_socket;
//        memcpy(&perHandleData->ssClientAddr, &client_address, client_len);
//        AssociateDeviceWithCompletionPort(iocp, (HANDLE)client_socket, (DWORD)perHandleData);
//        
//        char m_buffer[DATA_BUFSIZE];
//        uint32_t bts_recv = 0;
//        OVERLAPPED ov;
//        LPPER_IO_DATA per_io_data;
//        ZeroMemory(&(per_io_data->m_overlapped), sizeof(OVERLAPPED));
//        per_io_data->m_data_buf.len = DATA_BUFSIZE;
//        per_io_data->m_data_buf.buf = per_io_data->m_buffer;
//        per_io_data->m_ui_operation_type = RECV_POSTED;
//        WSARecv(perHandleData->hAcceptSocket, &per_io_data->m_data_buf, 1, (LPDWORD)&bts_recv, NULL, &per_io_data->m_overlapped, nullptr);
//    }
//
//	free(thread_handles);
//
//	if (TerminateWinsock() == CODE_FAIL)
//	{
//		return WSAGetLastError();
//	}
//	return 0;
//}
//
//uint32_t __stdcall ServerWorkerThread(void* completion_port_id)
//{
//    HANDLE completion_port = (HANDLE)completion_port_id;
//    uint32_t ui_bytes_transferred;
//    
//    LPOVERLAPPED overlapped;
//    LPPER_HANDLE_DATA per_handle_data;
//    LPPER_IO_DATA per_io_data;
//    uint32_t send_bytes;
//    uint32_t recv_bytes;
//    DWORD Flags;
//    while (TRUE)
//    {
//        // Ожидаем завершения операции ввода/вывода на любом сокете, 
//        // связанным с данным портом завершения
//        if (GetQueuedCompletionStatus(completion_port, (LPDWORD)&ui_bytes_transferred,
//            (PULONG_PTR)&per_handle_data, (LPOVERLAPPED*)&per_io_data, INFINITE) == 0)
//        {
//            // Сначала проверяем возврат на возможную ошибку. 
//            printf("GetQueuedCompletionStatus failed with error %d\n", GetLastError());
//            return 0;
//        }
//        // Если произошла ошибка типа BytesTransferred=0, что свидетельствует о
//        // закрытии сокета на удаленном хосте, закрываем свой сокет и очищаем данные, 
//        // связанные с сокетом
//        if (ui_bytes_transferred == 0 && (per_io_data->m_ui_operation_type == RECV_POSTED ||
//            per_io_data->m_ui_operation_type == SEND_POSTED))
//        {
//            closesocket(per_handle_data->hAcceptSocket);
//            GlobalFree(per_handle_data);
//            GlobalFree(per_io_data);
//            continue;//Продолжаем цикл 
//        }
//        // Обслуживаем завершенный запрос. Какая операция была закончена, определяем по 
//        // содержимому поля OperationTypefield в структуре PerIoData
//        if (per_io_data->m_ui_operation_type == RECV_POSTED)
//        {
//            printf("gg wp\n");
//            // Если тип операции был помечен как WSARecv(), выполняем необходимые действия с
//            // информацией, имеющейся в поле PerIoData->Buffer
//        }
//        // Выдаем следующий запрос на выполнение другой необходимой операции – WSASend()
//        // или WSARecv(). В нашем случае это WSARecv() – мы продолжаем получать данные
//        Flags = 0;
//        //Формируем данные для следующего  вызова операции с перекрытием
//        ZeroMemory(&(per_io_data->m_overlapped), sizeof(OVERLAPPED));
//        per_io_data->m_data_buf.len = DATA_BUFSIZE;
//        per_io_data->m_data_buf.buf = per_io_data->m_buffer;
//        per_io_data->m_ui_operation_type = RECV_POSTED;
//        //Выполняем вызов WSARecv() и переходим опять к ожиданию завершения 
//        WSARecv(per_handle_data->hAcceptSocket, &(per_io_data->m_data_buf), 1, (LPDWORD)&recv_bytes, &Flags,
//            &(per_io_data->m_overlapped), NULL);
//    }//End While 
//    return 1;
//}