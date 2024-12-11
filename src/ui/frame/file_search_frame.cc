#include "ui/frame/file_search_frame.hh"

FileSearchFrame::FileSearchFrame()
    : wxFrame(nullptr, wxID_ANY, LANG.Format("app_name", VERSION_STRING), wxDefaultPosition, wxSize(900, 600)),
    _core_count(std::thread::hardware_concurrency()),
    _async_file_proc(new AsyncFileProcessor()),
    _is_searching(false),
    _sorted_row(-1), 
    _ascending_sort(false) {
    
    wxPanel* panel = new wxPanel(this);

    InitializeUI(panel);
    ConfigureLayout(panel);
    ConfigureLayoutMacOS();
    BindEvents();

    _timer = new wxTimer(this);
}

FileSearchFrame::~FileSearchFrame() {
    delete _async_file_proc;
    delete _timer;
}

void FileSearchFrame::InitializeUI(wxPanel* panel) {
    _path_text_ctrl    = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));
    _core_count_ctrl   = new wxTextCtrl(panel, wxID_ANY, std::to_string(_core_count), wxDefaultPosition, wxSize(60, -1));
    _browse_btn        = new wxButton(panel, wxID_ANY, LANG.GetString("browse"));
    _search_btn        = new wxButton(panel, wxID_ANY, LANG.GetString("search"));

    _results_list_ctrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    _results_list_ctrl->InsertColumn(0, LANG.GetString("directory"), wxLIST_FORMAT_LEFT, 300);
    _results_list_ctrl->InsertColumn(1, LANG.GetString("file_folder_name"), wxLIST_FORMAT_LEFT, 200);
    _results_list_ctrl->InsertColumn(2, LANG.GetString("size"), wxLIST_FORMAT_LEFT, 100);
    _results_list_ctrl->InsertColumn(3, LANG.GetString("last_mod"), wxLIST_FORMAT_LEFT, 200);

    wxArrayString lang_choices;
    for (const auto& lang : LANG.GetAvailableLanguages()) {
        lang_choices.Add(lang);
    }

    _lang_combo_box = new wxComboBox(panel, wxID_ANY, lang_choices[0], wxDefaultPosition, wxDefaultSize, lang_choices, wxCB_READONLY);

    wxString def_lang      = LANG.GetLanguageNameFromCode(CFGMGR.def_cfg.def_lang);
    wxString selected_lang = CFGMGR.GetStoredString(CFGMGR.def_cfg.cfg_lang_path, def_lang);
    if (lang_choices.Index(selected_lang) != wxNOT_FOUND) {
        _lang_combo_box->SetStringSelection(selected_lang);
    }
}

void FileSearchFrame::ConfigureLayout(wxPanel* panel) {
    wxBoxSizer* vbox        = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox_top    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox_bottom = new wxBoxSizer(wxHORIZONTAL);

    vbox->Add(hbox_top, 0, wxEXPAND);
    vbox->Add(_results_list_ctrl, 1, wxEXPAND | wxALL, 5);
    vbox->Add(hbox_bottom, 0, wxALIGN_LEFT | wxALL, 5);

    hbox_top->Add(_path_text_ctrl, 1, wxEXPAND | wxALL, 5);
    hbox_top->Add(_core_count_ctrl, 0, wxEXPAND | wxALL, 5);
    hbox_top->Add(_browse_btn, 0, wxEXPAND | wxALL, 5);
    hbox_top->Add(_search_btn, 0, wxEXPAND | wxALL, 5);

    hbox_bottom->Add(_lang_combo_box, 0, wxALIGN_LEFT  | wxALL, 5);

    _curr_row_font_size = _uicfg.def_font_size;
   
    wxFont rows_font(_curr_row_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    _results_list_ctrl->SetFont(rows_font);
    _results_list_ctrl->Refresh();

    panel->SetSizer(vbox);
}

void FileSearchFrame::ConfigureLayoutMacOS() {
    wxMenuBar* menu_bar = new wxMenuBar();
    this->SetMenuBar(menu_bar);
}

void FileSearchFrame::BindEvents() {
    _search_btn->Bind(wxEVT_BUTTON, &FileSearchFrame::OnSearch, this);
    _browse_btn->Bind(wxEVT_BUTTON, &FileSearchFrame::OnBrowse, this);

    _results_list_ctrl->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FileSearchFrame::OnItemActivated, this);
    _results_list_ctrl->Bind(wxEVT_LIST_COL_CLICK, &FileSearchFrame::OnColumnClick, this);
    _results_list_ctrl->Bind(wxEVT_MOUSEWHEEL, &FileSearchFrame::OnMouseWheel, this);

    _lang_combo_box->Bind(wxEVT_COMBOBOX, &FileSearchFrame::OnLanguageChange, this);

    Bind(wxEVT_TIMER, &FileSearchFrame::OnTimer, this);
    Bind(wxEVT_CHAR_HOOK, &FileSearchFrame::OnKeyDown, this);
}

void FileSearchFrame::CopySelectedItemPath(long item_index) {
    if (item_index == -1) {
        return;
    }

    wxString directory     = _results_list_ctrl->GetItemText(item_index, 0);
    wxString file_name     = _results_list_ctrl->GetItemText(item_index, 1);
    wxString file_path     = DirectoryUtil::CreateFilePath(directory, file_name);
    wxString file_path_nrm = DirectoryUtil::NormalizePath(wxString(file_path));

    if (!wxTheClipboard->Open()) {
        wxMessageBox(LANG.GetString("clipboard_open_fail"), LANG.GetString("error"), wxICON_ERROR);
        return;
    }

    wxTheClipboard->SetData(new wxTextDataObject(file_path_nrm));
    wxTheClipboard->Close();
}

void FileSearchFrame::DeleteSelectedItem(long item_index) {
    if (item_index != -1) {
        _results_list_ctrl->DeleteItem(item_index);
    }
}

void FileSearchFrame::StartAsyncSearch() {
    if (_is_searching) {
        wxMessageBox(LANG.GetString("search_already_in_prog"), LANG.GetString("error"), wxICON_ERROR);
        return;
    }

    wxString path = _path_text_ctrl->GetValue();
    if (path.empty()) {
        wxMessageBox(LANG.GetString("path_search_missing"), LANG.GetString("error"), wxICON_ERROR);
        return;
    }

    unsigned int core_count = std::stoi(_core_count_ctrl->GetValue().ToStdString());
    if (core_count <= 0) {
        wxMessageBox(LANG.GetString("invalid_core_count"), LANG.GetString("error"), wxICON_ERROR);
        return;
    } 
    
    if (core_count > std::thread::hardware_concurrency()) {
        wxMessageBox(LANG.Format("core_less_or_equal", _core_count), LANG.GetString("error"), wxICON_ERROR);
        return;
    }

    _results_list_ctrl->DeleteAllItems();

    _async_file_proc->StartSearch(path, core_count);
    
    _is_searching = true;
    _timer->Start(_uicfg.update_time);
}

void FileSearchFrame::RefreshStaticUIElements() {
    wxListItem idir, i_ff_name, i_size, i_lastmod;
    idir.SetText(LANG.GetString("directory"));
    i_ff_name.SetText(LANG.GetString("file_folder_name"));
    i_size.SetText(LANG.GetString("size"));
    i_lastmod.SetText(LANG.GetString("last_mod"));

    _browse_btn->SetLabel(LANG.GetString("browse"));
    _search_btn->SetLabel(LANG.GetString("search"));
    
    _results_list_ctrl->SetColumn(0, idir);
    _results_list_ctrl->SetColumn(1, i_ff_name);
    _results_list_ctrl->SetColumn(2, i_size);
    _results_list_ctrl->SetColumn(3, i_lastmod);
}

void FileSearchFrame::OnBrowse(wxCommandEvent& event) {
    wxDirDialog dir_sel_dlg(this, LANG.GetString("file_picker_window_name"), "", wxDD_DEFAULT_STYLE);
    if (dir_sel_dlg.ShowModal() != wxID_OK) return;

    wxString sel_path = dir_sel_dlg.GetPath() + wxFILE_SEP_PATH;
    _path_text_ctrl->ChangeValue(sel_path);
}

void FileSearchFrame::OnKeyDown(wxKeyEvent& event) {
    if (event.ControlDown() && event.GetKeyCode() == 'C') {
        long item_index = _results_list_ctrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        CopySelectedItemPath(item_index);
    } else if (event.ControlDown() && event.GetKeyCode() == WXK_BACK) {
        long item_index = _results_list_ctrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        DeleteSelectedItem(item_index);
    } else if (event.GetKeyCode() == WXK_RETURN && _path_text_ctrl->HasFocus()) {
        StartAsyncSearch();
    } else {
        event.Skip();
    }
}

void FileSearchFrame::OnSearch(wxCommandEvent& event) {
    StartAsyncSearch();
}

void FileSearchFrame::OnItemActivated(wxListEvent& event) {
    long item_index = event.GetIndex();

    wxString directory      = _results_list_ctrl->GetItemText(item_index, 0);
    wxString file_name      = _results_list_ctrl->GetItemText(item_index, 1);
    wxString file_path      = DirectoryUtil::CreateFilePath(directory, file_name);
    wxString file_path_nrm  = DirectoryUtil::NormalizePath(wxString(file_path));
    
    bool show_res = FMInteraction::OpenFolderAndSelectFile(file_path_nrm.c_str());
    if (!show_res) {
        wxMessageBox(LANG.Format("show_file_fm_error", file_path_nrm.ToStdString()), LANG.GetString("error"), wxICON_ERROR);
    }
}

void FileSearchFrame::OnTimer(wxTimerEvent& event) {
    if (!_is_searching) {
        return;
    }

    auto results = _async_file_proc->GetResults();
    for (const auto& result : results) {
        long index = _results_list_ctrl->InsertItem(
            _results_list_ctrl->GetItemCount(), 
            DirectoryUtil::NormalizePath(result.directory)
        );

        wxString file_size_form = TypeConv::GetAppropriateSizeFormat(result.file_size);

        _results_list_ctrl->SetItem(index, 1, result.file_name);
        _results_list_ctrl->SetItem(index, 2, file_size_form);
        _results_list_ctrl->SetItem(index, 3, wxDateTime(result.last_modified).FormatISOCombined(' '));
    }

    if (_async_file_proc->IsDone()) {
        _is_searching = false;
        _timer->Stop();
        wxMessageBox(LANG.GetString("search_done"), LANG.GetString("info"), wxICON_INFORMATION);
    }
}

void FileSearchFrame::OnColumnClick(wxListEvent& event) {
    int col_id = event.GetColumn();

    if (_sorted_row == col_id) {
        _ascending_sort = !_ascending_sort;
    } else {
        _sorted_row = col_id;
        _ascending_sort = true;
    }

    std::vector<std::tuple<wxString, wxString, wxString, wxString>> items;
    for (long i = 0; i < _results_list_ctrl->GetItemCount(); ++i) {
        wxString directory     = _results_list_ctrl->GetItemText(i, 0);
        wxString file_name     = _results_list_ctrl->GetItemText(i, 1);
        wxString file_size     = _results_list_ctrl->GetItemText(i, 2);
        wxString last_modified = _results_list_ctrl->GetItemText(i, 3);

        items.emplace_back(directory, file_name, file_size, last_modified);
    }

    size_t item_size = items.size();
    int    items_cap = _algcfg.items_count_warning_cap;
    if (item_size > items_cap) {
        int user_choice = wxMessageBox(LANG.Format("more_than_x_in_list", items_cap, item_size), LANG.GetString("alg_warn"), wxICON_WARNING | wxYES_NO);
        if (user_choice != wxYES) return;
    }

    std::sort(items.begin(), items.end(), [this](const auto& a, const auto& b) {
        auto get_column_value = [this](const auto& item) -> wxString {
            switch (_sorted_row) {
            case 0: return std::get<0>(item);
            case 1: return std::get<1>(item);
            case 2: return std::get<2>(item);
            case 3: return std::get<3>(item);
            default: return wxString();
            }
        };

        const wxString& val_a = get_column_value(a);
        const wxString& val_b = get_column_value(b);

        return _ascending_sort ? val_a < val_b : val_a > val_b;
    });

    _results_list_ctrl->DeleteAllItems();
    for (const auto& item : items) {
        long index = _results_list_ctrl->InsertItem(
            _results_list_ctrl->GetItemCount(), std::get<0>(item)
        );

        _results_list_ctrl->SetItem(index, 1, std::get<1>(item));
        _results_list_ctrl->SetItem(index, 2, std::get<2>(item));
        _results_list_ctrl->SetItem(index, 3, std::get<3>(item));
    }
}

void FileSearchFrame::OnMouseWheel(wxMouseEvent& event) {
    if (!event.ControlDown()) {
        event.Skip();
        return;
    }

    int rotation = event.GetWheelRotation();
    if (rotation > 0 && _curr_row_font_size < _uicfg.font_size_max_rot) {
        _curr_row_font_size++;
    } else if (rotation < 0 && _curr_row_font_size > _uicfg.font_size_min_rot)  {
        _curr_row_font_size--;
    }

    wxFont rows_font(_curr_row_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    _results_list_ctrl->SetFont(rows_font);
    _results_list_ctrl->Refresh();
}

void FileSearchFrame::OnLanguageChange(wxCommandEvent& event) {
    wxString selected_lang_name = _lang_combo_box->GetValue();
    if (!LANG.SetLanguage(selected_lang_name)) {
        return;
    }

    CFGMGR.Update(CFGMGR.def_cfg.cfg_lang_path, selected_lang_name);

    RefreshStaticUIElements();
}