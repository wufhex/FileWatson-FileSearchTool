#pragma once
#include <wx/string.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

#include <unordered_map>
#include <string>
#include <fstream>

#include <nlohmann/json.hpp>

#include "util/dir_util.hh"

struct __unused_wxStringHash {
    std::size_t operator()(const wxString& str) const {
        return std::hash<std::string>()(std::string(str.ToStdString()));
    }
};

class LangHandler {
public:
    static LangHandler& Instance();

    bool LoadLanguages(const wxString& path, wxString& out_exception);
    bool SetLanguage(const wxString& language_name);

    template <typename... Args>
    wxString Format(const wxString& key, Args&&... args) const {
        const auto& str = GetString(key);
        return wxString::Format(str, std::forward<Args>(args)...);
    }

    wxString GetString(const wxString& key) const;

    wxString GetLanguageNameFromCode(const wxString& language_code) const;

    const std::vector<wxString>& GetAvailableLanguages() const;

private:
    LangHandler() = default;

    struct LangHandlerCfg {
        std::string lang_def_key = "lang_def";
    };

    LangHandlerCfg _lhcfg;
    
    std::map<wxString, wxString>                     _strings;
    std::map<wxString, std::map<wxString, wxString>> _lang_cache;
    std::map<wxString, wxString>                     _language_name_map;
    std::vector<wxString>                            _available_languages;
};

#define LANG (LangHandler::Instance())