#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#endif

#include <wx/string.h>
#include <wx/filename.h>

namespace DirectoryUtil {
#ifdef _WIN32
    bool IsDirectory(DWORD f_attr);
#else
    bool IsDirectory(const wxString& path);
#endif

    wxString CreateFilePath(const wxString& directory, const wxString& file_name);
    wxString NormalizePath(const wxString& in_path);
    wxString AddPathSpec(const wxString& in_path, const wxString& to_add);
    wxString RemovePathSpec(const wxString& in_path);
    wxString GetLastSegment(const wxString& in_path);

#ifdef __linux__
    std::string Linux_URLEncode(const std::string& input);
#endif
}