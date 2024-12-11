#include "lang/handler.hh"

LangHandler& LangHandler::Instance() {
    static LangHandler instance;
    return instance;
}

bool LangHandler::LoadLanguages(const wxString& path, wxString& out_exception) {
#ifdef __APPLE__
    wxString res_path = wxStandardPaths::Get().GetResourcesDir();
    wxString full_path = DirectoryUtil::AddPathSpec(res_path, path);

    wxDir dir(full_path);
#else
    wxDir dir(path);
#endif

    if (!dir.IsOpened()) {
        out_exception = "Failed to open directory: " + path;
        return false;
    }

    wxString filename;

    if (!dir.GetFirst(&filename, "*.json", wxDIR_FILES)) {
        out_exception = "No .json files found in directory: " + path;
        return false;
    }

    do {
        wxString file_path = dir.GetNameWithSep() + filename;

        std::ifstream file(file_path.ToStdString());
        if (!file.is_open()) {
            out_exception = "Missing or unreadable file: " + filename;
            return false;
        }

        nlohmann::json json;
        try {
            file >> json;
        }
        catch (const nlohmann::json::parse_error& e) {
            out_exception = "JSON parse error in file " + filename + ": " + wxString::FromUTF8(e.what());
            return false;
        }

        wxString lang_name = wxString::FromUTF8(json[_lhcfg.lang_def_key].get<std::string>().c_str());
        _language_name_map[filename.BeforeLast('.')] = lang_name;

        _available_languages.push_back(lang_name);
        for (const auto& [key, value] : json.items()) {
            if (key != _lhcfg.lang_def_key) {
                _lang_cache[lang_name][wxString::FromUTF8(key.c_str())] = wxString::FromUTF8(value.get<std::string>().c_str());
            }
        }
    } while (dir.GetNext(&filename));

    return true;
}

bool LangHandler::SetLanguage(const wxString& lang_name) {
    auto it = _lang_cache.find(lang_name);
    if (it == _lang_cache.end()) {
        return false;
    }

    _strings = it->second;
    return true;
}

wxString LangHandler::GetString(const wxString& key) const {
    auto it = _strings.find(key);
    if (it == _strings.end()) {
        return "String not found";
    }

    return it->second;
}

const std::vector<wxString>& LangHandler::GetAvailableLanguages() const {
    return _available_languages;
}

wxString LangHandler::GetLanguageNameFromCode(const wxString& language_code) const {
    auto it = _language_name_map.find(language_code);
    if (it != _language_name_map.end()) {
        return it->second;
    }
    return "";
}