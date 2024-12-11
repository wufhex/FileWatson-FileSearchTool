#pragma once
#include "ui/frame/file_search_frame.hh"
#include "res.h"

class FileSearchApp : public wxApp {
public:
    virtual bool OnInit() override;

private:
    struct MainAppCfg {
        wxString lang_folder = "lang";
    };

    MainAppCfg       _app_cfg;
    FileSearchFrame* _frame;

    bool LoadLangFile();
    void Win_LoadIcon();
};