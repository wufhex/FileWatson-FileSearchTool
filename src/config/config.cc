#include "config/config.hh"

ConfigMgr& ConfigMgr::Instance() {
	static ConfigMgr instance;
	return instance;
}

ConfigMgr::ConfigMgr() {
	_config = wxConfigBase::Get();
}

void ConfigMgr::Update(wxString cfg_path, wxString lang_code) {
	_config->Write(cfg_path, lang_code);
}

wxString ConfigMgr::GetStoredString(wxString cfg_path, wxString _default) {
	wxString lang_code;
	_config->Read(cfg_path, &lang_code, _default);

	return lang_code;
}