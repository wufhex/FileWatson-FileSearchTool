#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/timer.h>
#include <wx/defs.h>
#include <wx/clipbrd.h>
#include <wx/combobox.h>

#include "file_processor/async_file_processor.hh"
#include "util/fm_interaction.hh"
#include "lang/handler.hh"
#include "config/config.hh"
#include "ver.hh"

class FileSearchFrame : public wxFrame {
public:
    FileSearchFrame();
    ~FileSearchFrame();

private:
    struct UIConfig {
        /* Milliseconds */
        int update_time       = 500;
        
        int def_font_size     = 9;
        
        /* Used for CTRL + Scroll zoom */
        int font_size_max_rot = 20;
        int font_size_min_rot = 6;
    };

    struct AlgorithmConfig {
        int items_count_warning_cap = 5000;
    }; 

    UIConfig        _uicfg;
    AlgorithmConfig _algcfg;

    wxTextCtrl* _path_text_ctrl;
    wxTextCtrl* _core_count_ctrl;
    wxButton*   _search_btn;
    wxButton*   _browse_btn;
    wxListCtrl* _results_list_ctrl;
    wxComboBox* _lang_combo_box;

    AsyncFileProcessor* _async_file_proc;
    wxTimer* _timer;

    unsigned int _core_count;
    bool         _is_searching;
    
    int          _sorted_row;
    bool         _ascending_sort;
    
    int          _curr_row_font_size;

    void InitializeUI(wxPanel* panel);
    void ConfigureLayout(wxPanel* panel);
    void ConfigureLayoutMacOS();
    void BindEvents();

    void CopySelectedItemPath(long item_index);
    void DeleteSelectedItem(long item_index);
    void StartAsyncSearch();
    void RefreshStaticUIElements();

    void OnBrowse(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnColumnClick(wxListEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnLanguageChange(wxCommandEvent& event);
};