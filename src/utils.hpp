/* utils.hpp - contains declarations for the utilities to be used by NTPDRAPE-II */

#pragma once
#include "common.hpp"

namespace Utils {

	namespace Resource {

		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName, DWORD CreateFileDisposition);

		bool ExtractFromResource(const wchar_t* dest, LPCWSTR resName);
	
	}
}
