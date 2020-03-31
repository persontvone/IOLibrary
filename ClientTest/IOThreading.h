#pragma once
#include "Common.h"

/*
	Call this function to create a new completion port.
	@param [in] number_of_concurrent_threads - The maximum number of threads that the operating system can allow
	to concurrently process I/O completion packets for the I/O completion port.
	@return If the function succeeds, the return value is the handle to an I/O completion port, otrherwise NULL.
*/
HANDLE GetCompletionPortHandle(DWORD number_of_concurrent_threads);

/*
	Call this function to append an entry to an existing completion port's device list.
	@param [in] completion_port - A handle to an existing I/O completion port.
	@param [in] device_handle - An open file handle. If a handle is provided, it has to have been opened for overlapped I/O completion.
	@param [in] completion_key - The per-handle user-defined completion key that is included in every I/O completion packet for the specified file handle.
*/
HANDLE AssociateDeviceWithCompletionPort(HANDLE completion_port, HANDLE device_handle, DWORD completion_key);