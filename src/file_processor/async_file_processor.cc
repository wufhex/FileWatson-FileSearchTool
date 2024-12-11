#include "file_processor/async_file_processor.hh"

AsyncFileProcessor::AsyncFileProcessor()
    : _done(false) {
}

void AsyncFileProcessor::StartSearch(const wxString& path, int num_threads) {
    wxString fixed_path = DirectoryUtil::NormalizePath(path);

    _done = false;
    _search_future = std::async(std::launch::async, &AsyncFileProcessor::SearchWorker,
        this, fixed_path, num_threads);
}

std::vector<FileSearchResult> AsyncFileProcessor::GetResults() {
    std::lock_guard<std::mutex> lock(_results_mutex);

    std::vector<FileSearchResult> results;
    while (!_results_queue.empty()) {
        results.push_back(_results_queue.front());
        _results_queue.pop();
    }

    return results;
}

bool AsyncFileProcessor::IsDone() const {
    return _done;
}

void AsyncFileProcessor::SearchWorker(const wxString& path, int num_threads) {
    auto callback = [this](
        const wxString& directory,
        const wxString& file_name,
        uint64_t file_size,
        time_t last_modified,
        bool is_directory
        ) {
            FileSearchResult result
                = { directory, file_name, file_size, last_modified, is_directory };

            std::lock_guard<std::mutex> lock(_results_mutex);
            _results_queue.push(result);
        };

    _file_processor.SearchFilesMultithreaded(path, callback, num_threads);
    _done = true;
}