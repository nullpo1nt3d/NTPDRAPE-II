/* utils.hpp - contains declarations for the utilities to be used by NTPDRAPE-II */

#pragma once
#include <windef.h>

namespace Utils {

	namespace MIDI {
		extern UINT midiDeviceId;
		DWORD PlayMIDIFromPath(HWND hwnd, const wchar_t* path);
	}

	namespace Resource {

		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName, DWORD CreateFileDisposition);

		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName);
	
	}
}
