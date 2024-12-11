#pragma once
#include <functional>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <ctime>

#include "util/dir_util.hh"
#include "util/pattern.hh"
#include "util/type_conversion.hh"

using SearchCallback = std::function<void(const wxString&, const wxString&, uint64_t, time_t, bool)>;

class FileProcessor {
public:
    FileProcessor();

    void SearchFilesMultithreaded(const wxString& path,
        const SearchCallback& callback, int num_threads);

private:
    std::mutex                   _queue_mutex;
    std::condition_variable      _queue_cv;
    std::queue<wxString>         _directory_queue;
    std::atomic<int>             _active_threads;
    std::atomic<bool>            _stop_flag;

    void ProcessDirectory(const wxString& directory_path, const wxString& pattern,
        const SearchCallback& callback);

    void WorkerThread(const wxString& pattern,
        const SearchCallback& callback);
};