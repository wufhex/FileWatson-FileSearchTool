#pragma once
#include <string>

#ifdef _WIN32
#include <Windows.h>
#include <shlobj.h>
#elif __APPLE__
#include <wx/utils.h>
#elif __linux__ 
#include <dbus/dbus.h>
#endif

#include "type_conversion.hh"
#include "dir_util.hh"

namespace FMInteraction {
	bool OpenFolderAndSelectFile(const wxString& path);
}