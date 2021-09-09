/* main.cpp - contains the entry point for NTPDRAPE-II */

#include "common.hpp"
#include "Resource.h"
#include <process.h>
#include <shlobj.h>
#include <windowsx.h>
#include <winternl.h>

HINSTANCE hInst;
HWND main_dlg_window;
std::vector<HANDLE> InvokeNtpdThreadHandles;

UINT StaticControlIDs[] = {
	BeginEnd_Button,
	DisableBtn_CheckBox,
	ThreadsToRun_EditBox
};

const wchar_t* StaticControlTooltips[] = {
	L"Start Notepad Rape",
	L"Disable start button once pressed (point of no return)",
	L"Amount of Notepad invoking threads to run"
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
		ti.uId = reinterpret_cast<UINT_PTR>(GetDlgItem(OwnerWnd, StaticControlIDs[i]));
		ti.lpszText = const_cast<wchar_t*>(StaticControlTooltips[i]);
		SendMessage(tooltipWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}
}

void BeginEndNtpdRape(HWND ownerWnd)
{
	static bool isNtpdRapeRunning = false;
	UINT threadsToRun = GetDlgItemInt(ownerWnd, ThreadsToRun_EditBox, nullptr, false);
	if (threadsToRun == 0)
		return;

	if (IsDlgButtonChecked(ownerWnd, DisableBtn_CheckBox))
		EnableWindow(GetDlgItem(ownerWnd, BeginEnd_Button), false);

	EnableWindow(GetDlgItem(ownerWnd, ThreadsToRun_EditBox), isNtpdRapeRunning);

	if (!isNtpdRapeRunning)
	{
		static bool isMidiExtracted = false;
		static wchar_t extractPath[MAX_PATH];

		// Extract midi if it isn't already extracted
		if (!isMidiExtracted)
		{
			SHGetSpecialFolderPath(ownerWnd, extractPath, CSIDL_COMMON_APPDATA, false);
			wcscat(extractPath, L"\\Mid0.mid");

			if (Utils::Resource::ExtractFromResource(extractPath, MAKEINTRESOURCE(ResourceMidi_File)))
				isMidiExtracted = true;
		}

		// For some reason, this process takes a while to complete
		// and stalls program operation for ~ 2 seconds.
		if (isMidiExtracted)
			Utils::MIDI::PlayMIDIFromPath(ownerWnd, extractPath);

#ifndef _DEBUG
		// Create the notepad invoking threads
		for (int threadsRunning = 0; threadsRunning <= threadsToRun; threadsRunning++)
			InvokeNtpdThreadHandles.push_back(CreateThread(nullptr, 0, InvokeNotepad, nullptr, 0, nullptr));
#endif // _DEBUG

		isNtpdRapeRunning = true;
		SetDlgItemText(ownerWnd, BeginEnd_Button, L"End");
	}
			
	else
	{
		
#ifndef _DEBUG
		// Scroll through the thread handles and terminate them
		for (int threads = threadsToRun; threads >= 0; threads--)
			TerminateThread(InvokeNtpdThreadHandles.at(threads), 0);
#endif

		// Stop the midi
		mciSendCommand(Utils::MIDI::midiDeviceId, MCI_CLOSE, 0, 0);

		isNtpdRapeRunning = false;
		SetDlgItemText(ownerWnd, BeginEnd_Button, L"Begin");
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
	
		case MM_MCINOTIFY:
		{
			mciSendCommand(Utils::MIDI::midiDeviceId, MCI_CLOSE, 0, 0);
			break;
		}

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
					BeginEndNtpdRape(hwnd);
					break;
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