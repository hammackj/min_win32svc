#include <windows.h>
#include <tchar.h>

#define SERVICE_NAME _T("min_win32svc")

SERVICE_STATUS svc_status = {0};
SERVICE_STATUS_HANDLE svc_status_handle = NULL;
HANDLE svc_stop_event = INVALID_HANDLE_VALUE;

VOID WINAPI svc_main(DWORD argc, LPTSTR * argv);
VOID WINAPI svc_ctrl_handler(DWORD ctrl_code);
DWORD WINAPI svc_worker_thread(LPVOID param);

/**
 *
 */
int _tmain(int argc, TCHAR * argv[])
{
	SERVICE_TABLE_ENTRY svc_table[] = {
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) svc_main},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(svc_table) == FALSE) {
		OutputDebugString(_T("Service error"));

		return GetLastError();
	}

	return 0;
}

/**
 *
 */
VOID WINAPI svc_main(DWORD argc, LPTSTR * argv)
{
	DWORD status = E_FAIL;

	svc_status_handle = RegisterServiceCtrlHandler(SERVICE_NAME,
			svc_ctrl_handler);

	if (svc_status_handle == NULL)
		return;

	ZeroMemory(&svc_status, sizeof(svc_status));
	svc_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	svc_status.dwControlsAccepted = 0;
	svc_status.dwCurrentState = SERVICE_START_PENDING;
	svc_status.dwWin32ExitCode = 0;
	svc_status.dwServiceSpecificExitCode = 0;
	svc_status.dwCheckPoint = 0;

	/* Check for false?? check msdn*/
	SetServiceStatus(svc_status_handle, &svc_status);

	svc_stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (svc_stop_event == NULL) {
		svc_status.dwControlsAccepted = 0;
		svc_status.dwCurrentState = SERVICE_STOPPED;
		svc_status.dwWin32ExitCode = GetLastError();
		svc_status.dwCheckPoint = 1;

		SetServiceStatus(svc_status_handle, &svc_status);

		return;
	}

	svc_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	svc_status.dwCurrentState = SERVICE_RUNNING;
	svc_status.dwWin32ExitCode = 0;
	svc_status.dwCheckPoint = 0;

	SetServiceStatus(svc_status_handle, &svc_status);

	HANDLE thread = CreateThread(NULL, 0, svc_worker_thread, NULL, 0, NULL);

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(svc_stop_event);

	svc_status.dwControlsAccepted = 0;
	svc_status.dwCurrentState = SERVICE_STOPPED;
	svc_status.dwWin32ExitCode = 0;
	svc_status.dwCheckPoint = 3;

	SetServiceStatus(svc_status_handle, &svc_status);
}

/**
 *
 */
VOID WINAPI svc_ctrl_handler(DWORD ctrl_code)
{
	switch (ctrl_code) {
	case SERVICE_CONTROL_STOP:
		if (svc_status.dwCurrentState != SERVICE_RUNNING)
			break;

		svc_status.dwControlsAccepted = 0;
		svc_status.dwCurrentState = SERVICE_STOP_PENDING;
		svc_status.dwWin32ExitCode = 0;

		SetServiceStatus(svc_status_handle, &svc_status);

		SetEvent(svc_stop_event);

		break;

	default:
		break;
	}
}

/**
 *
 */
DWORD WINAPI svc_worker_thread(LPVOID param)
{
	while (WaitForSingleObject(svc_stop_event, 0) != WAIT_OBJECT_0) {
		/* Service Code here*/

		Sleep(3000);
	}

	return ERROR_SUCCESS;
}

