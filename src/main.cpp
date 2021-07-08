/* main.cpp - contains the entry point for NTPDRAPE-II */

#include "Resource.h"
#include <process.h>
#include <shlobj.h>
#include <vector>
#include <windows.h>
#include <windowsx.h>
#include <winternl.h>

HINSTANCE hInst;
HWND main_dlg_window;
std::vector<HANDLE> InvokeNtpdThreadHandles;

UINT StaticControlIDs[] = {
	BeginEnd_Button,
	DisableBtn_CheckBox
};

wchar_t* StaticControlTooltips[] = {
	L"Start Notepad Rape",
	L"Disable start button once pressed (point of no return)"
};

//------------------------------------------------ Definitions

DWORD WINAPI InvokeNotepad(LPVOID)
{
	static int TotalInstances = 0;
	while (true)
	{
		// // Sanity Check
		// if (GetKeyState(VK_CONTROL) < 0)
		// 	continue;

		// Execute notepad but with no window owner, since it could easily end everything.
		if ((int)ShellExecute(nullptr, L"open", L"notepad.exe", nullptr, nullptr, SW_SHOW) > 32) // NOLINT: ShellExecute does NOT return a true HINSTANCE
		{
			TotalInstances++;
			SetDlgItemInt(main_dlg_window, InstancesCount, TotalInstances, false);
		}
	}
	return 0;
}

void CreateControlTooltips(HWND OwnerWnd)
{
	HWND tooltipWnd = CreateWindowEx(0, TOOLTIPS_CLASS, nullptr, WS_POPUP | TTS_USEVISUALSTYLE | TTS_ALWAYSTIP | TTS_BALLOON,
									 0, 0, 0, 0, OwnerWnd, nullptr, ::hInst, nullptr);

	if (!tooltipWnd)
		return;

	TOOLINFO ti{};
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = OwnerWnd;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;

	// Array size of control IDs is directly proportional
	// to that of the control's tooltip text array.
	for (int i = 0; i < ARRAYSIZE(StaticControlIDs); i++)
	{
		ti.uId = (UINT_PTR)GetDlgItem(OwnerWnd, StaticControlIDs[i]);
		ti.lpszText = StaticControlTooltips[i];
		SendMessage(tooltipWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}
}


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT desktopRect, wndRect;
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			
			CreateControlTooltips(hwnd);

			// Center window position
			GetWindowRect(GetDesktopWindow(), &desktopRect);
			GetWindowRect(hwnd, &wndRect);
			SetWindowPos(hwnd, HWND_TOPMOST,
	                 	(desktopRect.right + desktopRect.left) / 2 - (wndRect.right - wndRect.left) / 2,
	                 	(desktopRect.top + desktopRect.bottom) / 2 - (wndRect.bottom - wndRect.top) / 2, 0, 0, SWP_NOSIZE);

			SetDlgItemInt(hwnd, InstancesCount, 0, false);
	
			return true;
	
		case WM_COMMAND:
	
			switch(LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
				{
					PostQuitMessage(0);
					return true;
				}
				break;

				case BeginEnd_Button:
				{
					static bool InvokeNtpdRunning = false;
					UINT threadsToRun = 5; // temporary default
	
					if (IsDlgButtonChecked(hwnd, DisableBtn_CheckBox))
						EnableWindow(GetDlgItem(hwnd, BeginEnd_Button), 0);
			
					if (!InvokeNtpdRunning)
					{
						InvokeNtpdRunning = true;
						SetDlgItemText(hwnd, BeginEnd_Button, L"End");
	
						// Create the notepad invoking threads
						for (int threadsRunning = 0; threadsRunning <= threadsToRun; threadsRunning++)
							InvokeNtpdThreadHandles.push_back(CreateThread(nullptr, 0, InvokeNotepad, nullptr, 0, nullptr));
					}
			
					else
					{
						InvokeNtpdRunning = false;
			
						// Scroll through the thread handles and terminate them
						for (int threads = threadsToRun; threads >= 0; threads--)
							TerminateThread(InvokeNtpdThreadHandles.at(threads), 0);
			
						SetDlgItemText(hwnd, BeginEnd_Button, L"Begin");
					}
				}
				break;
			}
			break;
	}
	return false;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                   	 LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Initialize common controls and create the dialog window
	InitCommonControls();
	::hInst = hInstance;
	main_dlg_window = CreateDialog(hInstance, MAKEINTRESOURCE(Main_Dlg), nullptr, DlgProc);
	if (!main_dlg_window)
		return 0;

	// Show window and set the icon
	ShowWindow(main_dlg_window, SW_SHOW);
	HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(NtpdRapeII_Icon));
	SendMessage(main_dlg_window, WM_SETICON, ICON_BIG, (LPARAM)icon);

	// Message handling loop
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}