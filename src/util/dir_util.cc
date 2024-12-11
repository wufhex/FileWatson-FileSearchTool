#include "util/dir_util.hh"

#ifdef _WIN32
bool DirectoryUtil::IsDirectory(DWORD f_attr) {
    return (f_attr != INVALID_FILE_ATTRIBUTES && (f_attr & FILE_ATTRIBUTE_DIRECTORY));
}
#else
bool DirectoryUtil::IsDirectory(const wxString& path) {
    struct stat stat_buf;
    if (stat(path.ToStdString().c_str(), &stat_buf) != 0) {
        return false;
    }
    return S_ISDIR(stat_buf.st_mode);
}
#endif

wxString DirectoryUtil::CreateFilePath(const wxString& directory, const wxString& file_name) {
    wxFileName file(directory, file_name);

    return file.GetFullPath();
}

wxString DirectoryUtil::NormalizePath(const wxString& in_path) {
    wxFileName file_name(in_path);
    file_name.MakeAbsolute();

#ifdef _WIN32
    wxString path_str = file_name.GetFullPath();
    path_str.Replace('/', '\\');
#endif

    wxString full_path = file_name.GetFullPath();
    return full_path;
}

wxString DirectoryUtil::AddPathSpec(const wxString& in_path, const wxString& to_add) {
    wxString res_path = in_path;
    if (!res_path.EndsWith(wxFILE_SEP_PATH)) {
        res_path += wxFILE_SEP_PATH;
    }

    res_path += to_add;
    return res_path;
}

wxString DirectoryUtil::RemovePathSpec(const wxString& in_path) {
    if (in_path.IsEmpty()) {
        return in_path;
    }

    size_t last_sep_pos = in_path.rfind(wxFILE_SEP_PATH);
    if (last_sep_pos != wxNOT_FOUND) {
        return in_path.SubString(0, last_sep_pos);
    }

    return in_path;
}

wxString DirectoryUtil::GetLastSegment(const wxString& in_path) {
    wxString last_segment;
    if (!in_path.IsEmpty()) {
        last_segment = in_path.AfterLast(wxFILE_SEP_PATH);
    }

    return last_segment;
}

#ifdef __linux__
std::string DirectoryUtil::Linux_URLEncode(const std::string& input) {
    std::ostringstream encoded;
    for (wxChar c : input) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/') {
            encoded << (char)c;
        }
        else {
            encoded << '%' << std::hex << std::uppercase << int((unsigned char)c);
        }
    }

    return "file://" + encoded.str();
}
#endif