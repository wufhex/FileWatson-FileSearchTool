#pragma once
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <wx/string.h>
#include <string>

#include <algorithm>

namespace TypeConv {
	uint64_t CombineU32(uint32_t hi, uint32_t lo);
	wxString GetAppropriateSizeFormat(uint64_t bytes);
#ifdef _WIN32
	time_t   Win_ToUnixTimestamp(ULONGLONG win_time);
#endif
}