#include <Windows.h>
#include <stdio.h>

#define MAX_SERVER_TH 64
#define MAX_CLIENTS_CP 1024

typedef struct
{
	HANDLE hCompPort;
	DWORD threadNum;
} SERVER_THREAD_ARG, *LPSERVER_THREAD_ARG;

enum CP_CLIENT_PIPE_STATE
{
	connected, requested, computed, responding, respondLast
};

typedef struct
{
	HANDLE hCompPort;
	HANDLE hNp;
	HANDLE hTempFile;
	FILE* tFp;
	TCHAR tmpfileName[MAX_PATH];
	REQUEST request;
	DWORD nBytes;
	enum CP_CLIENT_PIPE_STATE npState;
	LPOVERLAPPED pOverLap;
} CP_KEY;

OVERLAPPED overLap;
volatile static int shutDown = 0;
static DWORD WINAPI Server(LPSERVER_THREAD_ARG);
static DWORD WINAPI ServerBroadcast(LPLONG);
static BOOL WINAPI Handler(DWORD);
static DWORD WINAPI ComputeThread(PVOID);

static CP_KEY Key[MAX_CLIENTS_CP];

int main(int argc, char** argv)
{
	HANDLE hCompPort, hMonitor, hServerThread[MAX_CLIENTS_CP];
	DWORD iNp, iTh;
	SECURITY_ATTRIBUTES tempFileSA = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	SERVER_THREAD_ARG ThArgs[MAX_SERVER_TH];
	OVERLAPPED ov = { 0 };

	SetConsoleCtrlHandler(Handler, true);

	hMonitor = (HANDLE)_beginthreadex(NULL, 0, ServerBroadcast, NULL, 0, NULL);

	hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, MAX_SERVER_TH);

	for (iNp = 0; iNp < MAX_CLIENTS_CP; iNp++)
	{
		memset(&Key[iNp], 0, sizeof(CP_KEY));
		Key[iNp].hCompPort = hCompPort;
		Key[iNp].hNp = CreateNamedPipe(SERVER_PIPE, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE, MAX_CLIENTS_CP, 0, 0,
			INFINITE, &tempFileSA);
	}
}
