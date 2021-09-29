/* utils.cpp - contains the utilities to be used by NTPDRAPE-II */

#include "common.hpp"
#include <filesystem>


namespace Utils {

	namespace MIDI {
		UINT midiDeviceId;
		DWORD PlayMIDIFromPath(HWND hwnd, const wchar_t* path)
		{
			MCI_OPEN_PARMS mciOpenParms;
			mciOpenParms.lpstrDeviceType = L"sequencer";
			mciOpenParms.lpstrElementName = path;

			// Open the midi device
			DWORD dwReturn;
			if ((dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, reinterpret_cast<DWORD_PTR>(&mciOpenParms))))
				return LOWORD(dwReturn);

			midiDeviceId = mciOpenParms.wDeviceID;

			MCI_STATUS_PARMS mciStatusParms;
			mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;

			// Set the sequencer status
			if ((dwReturn = mciSendCommand(midiDeviceId, MCI_STATUS, MCI_STATUS_ITEM, reinterpret_cast<DWORD_PTR>(&mciStatusParms))))
			{
				mciSendCommand(midiDeviceId, MCI_CLOSE, 0, 0);
				return LOWORD(dwReturn);
			}

			MCI_PLAY_PARMS mciPlayParms;
			mciPlayParms.dwCallback = reinterpret_cast<DWORD_PTR>(hwnd);

			// Play the midi
			if ((dwReturn = mciSendCommand(midiDeviceId, MCI_PLAY, MCI_NOTIFY, reinterpret_cast<DWORD_PTR>(&mciPlayParms))))
			{
				mciSendCommand(midiDeviceId, MCI_CLOSE, 0, 0);
				return LOWORD(dwReturn);
			}

			return S_OK;
		}
	}
	
	namespace Resource
	{
		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName, DWORD CreateFileDisposition)
		{
			// Don't waste time if we already extracted
			if (std::filesystem::exists(dest))
				return true;
            
			bool extract_ok = false;
            DWORD resSize = 0;
            HGLOBAL hglobal = nullptr;
            HRSRC rsrc = nullptr;
            LPVOID rsrcData = nullptr;

            // Find the resource
            rsrc = FindResource(nullptr, resName, RT_RCDATA);
            if (!rsrc)
                goto exit;
            
            // Get the size
            resSize = SizeofResource(nullptr, rsrc);
            if (!resSize)
                goto exit;

            // Lock the resource (get a pointer to the resource)
            rsrcData = LockResource(LoadResource(nullptr, rsrc));
            if (!rsrcData)
                goto exit;
	
			// Allocate memory for the resource
		    hglobal = GlobalAlloc(GMEM_MOVEABLE, resSize);
		    if (hglobal)
		    {
		        LPVOID extractBuffer = GlobalLock(hglobal);
		        if (extractBuffer)
		        {
		            memcpy(extractBuffer, rsrcData, resSize);

		            HANDLE file = CreateFileW(dest, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CreateFileDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
		            if (file)
		            {
		                DWORD bytesWritten;
		                if(WriteFile(file, extractBuffer, resSize, &bytesWritten, nullptr)
		                   &&
		                   CloseHandle(file))
		                   extract_ok = true;
		            }
		            GlobalUnlock(hglobal);
		        }
		        GlobalFree(hglobal);
		    }

		    exit:
		  	return extract_ok;
		}
	
		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName)
		{
			// Default to create new file
			return ExtractFromResource(dest, resName, CREATE_NEW);
		}
	}
}