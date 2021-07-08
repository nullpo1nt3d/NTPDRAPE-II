/* utils.cpp - contains the utilities to be used by NTPDRAPE-II */

#include "common.hpp"

namespace Utils {
	
	namespace Resource
	{
		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName, DWORD CreateFileDisposition)
		{
		    HRSRC rsrc = FindResource(nullptr, resName, RT_RCDATA);
		    if (!rsrc)
		        return false;
		        
		    DWORD resSize = SizeofResource(nullptr, rsrc);
		    if (!resSize)
		        return false;
		    
		    LPVOID rsrcData = LockResource(LoadResource(nullptr, rsrc));
		    if (!rsrcData)
		        return false;
	
		    HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, resSize);
		    if (hglobal)
		    {
		        LPVOID extractBuffer = GlobalLock(hglobal);
		        if (extractBuffer)
		        {
		            memcpy(extractBuffer, rsrcData, resSize);
	
		            // utils.cpp -
		            // was already extracted.
		            HANDLE file = CreateFileW(dest, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CreateFileDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
		            if (GetLastError() == ERROR_FILE_EXISTS)
		                return true;
	
		            else if (file)
		            {
		                DWORD bytesWritten;
		                if(WriteFile(file, extractBuffer, resSize, &bytesWritten, nullptr)
		                   &&
		                   CloseHandle(file))
		                   return true; // Would this memleak???
		            }
		            GlobalUnlock(hglobal);
		        }
		        GlobalFree(hglobal);
		    }
		  	return false;
		}
	
		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName)
		{
			// Default to create new file
			return ExtractFromResource(dest, resName, CREATE_NEW);
		}
	}
}