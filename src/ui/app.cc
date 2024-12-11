#include "ui/app.hh"

bool FileSearchApp::OnInit() {
    if (!LoadLangFile()) {
        return false;
    }

    _frame = new FileSearchFrame();
    Win_LoadIcon();

    _frame->Show(true);
    return true;
}

bool FileSearchApp::LoadLangFile() {
    wxString lang_exception;

    bool load_res = LANG.LoadLanguages(_app_cfg.lang_folder, lang_exception);
    if (!load_res) {
        if (!lang_exception.empty()) {
            wxMessageBox(lang_exception, "Failed to load language files", wxICON_ERROR);
        }

        return false;
    }

    wxString def_lang = LANG.GetLanguageNameFromCode(CFGMGR.def_cfg.def_lang);
    wxString lang_name = CFGMGR.GetStoredString(CFGMGR.def_cfg.cfg_lang_path, def_lang);
    if (lang_name.empty()) {
        wxMessageBox("Failed to load language", "Error", wxICON_ERROR);
    }

    LANG.SetLanguage(lang_name);

    return true;
}

void FileSearchApp::Win_LoadIcon() {
#ifdef _WIN32
    wxIcon app_icon;
    app_icon.LoadFile(APP_ICON, wxBITMAP_TYPE_ICO_RESOURCE);

    if (app_icon.IsOk()) {
        _frame->SetIcon(app_icon);
    }
#endif
}