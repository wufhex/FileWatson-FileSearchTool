#pragma once
#include <wx/string.h> 
#include <future>

#include "file_processor.hh"

struct FileSearchResult {
    wxString directory;
    wxString file_name;
    uint64_t file_size;
    time_t   last_modified;
    bool     is_directory;
};

class AsyncFileProcessor {
public:
    AsyncFileProcessor();

    void StartSearch(const wxString& path, int num_threads);

    std::vector<FileSearchResult> GetResults();

    bool IsDone() const;

private:
    FileProcessor                 _file_processor;
    std::future<void>             _search_future;
    std::atomic<bool>             _done; 
    std::mutex                    _results_mutex;
    std::queue<FileSearchResult>  _results_queue;

    void SearchWorker(const wxString& path, int num_threads);
};