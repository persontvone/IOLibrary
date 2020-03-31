#include "IOThreading.h"

HANDLE GetCompletionPortHandle(DWORD number_of_concurrent_threads)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, number_of_concurrent_threads);
}

HANDLE AssociateDeviceWithCompletionPort(HANDLE completion_port, HANDLE device_handle, DWORD completion_key)
{
	HANDLE h = CreateIoCompletionPort(device_handle, completion_port, completion_key, 0);
	return h;
}
