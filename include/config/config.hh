#pragma once
#include <wx/config.h>
#include <wx/string.h>

class ConfigMgr {
public:
	struct DefCfg {
		wxString def_lang     = "en_us";

		wxString cfg_lang_path = "/watson/language";
	};
	
	static ConfigMgr& Instance();

	void     Update(wxString cfg_path, wxString lang_code);
	wxString GetStoredString(wxString cfg_path, wxString _default);

	DefCfg def_cfg;
private:
	ConfigMgr();

	wxConfigBase* _config;
};

#define CFGMGR (ConfigMgr::Instance())