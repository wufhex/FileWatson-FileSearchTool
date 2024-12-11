#include "file_processor/file_processor.hh"

FileProcessor::FileProcessor()
    : _active_threads(0), _stop_flag(false) {
}

void FileProcessor::SearchFilesMultithreaded(const wxString& path,
    const SearchCallback& callback, int num_threads) {
    PatternUtil::SearchData search_data = PatternUtil::PathToSearchData(path);
    if (!search_data.valid) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_queue_mutex);
        _directory_queue.push(search_data.path);
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, search_data, callback]() {
            WorkerThread(search_data.term, callback);
            });
    }

    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _queue_cv.wait(lock, [this] {
            return _directory_queue.empty() && _active_threads == 0;
        });
    }

    _stop_flag = true;
    _queue_cv.notify_all();

    for (std::thread& thread : threads) {
        thread.join();
    }

    _stop_flag = false;
}

void FileProcessor::ProcessDirectory(const wxString& directory_path, const wxString& pattern,
    const SearchCallback& callback) {
#ifdef _WIN32
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW((directory_path + L"\\*").c_str(), &find_data);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        wchar_t* file_name = find_data.cFileName;
        if (wcscmp(file_name, L".") == 0 || wcscmp(file_name, L"..") == 0) continue;

        wxString full_path = directory_path + wxFileName::GetPathSeparator() + file_name;
        bool is_directory = DirectoryUtil::IsDirectory(find_data.dwFileAttributes);

        uint64_t file_size = TypeConv::CombineU32(
            find_data.nFileSizeHigh,
            find_data.nFileSizeLow
        );

        ULARGE_INTEGER ull;
        FILETIME file_time_last_write = find_data.ftLastWriteTime;
        ull.LowPart  = file_time_last_write.dwLowDateTime;
        ull.HighPart = file_time_last_write.dwHighDateTime;

        time_t edit_timestamp = TypeConv::Win_ToUnixTimestamp(ull.QuadPart);

        if (is_directory) {
            if (PatternUtil::MatchesPattern(file_name, pattern)) {
                callback(directory_path, file_name, file_size, edit_timestamp, true);
            }

            std::lock_guard<std::mutex> lock(_queue_mutex);
            _directory_queue.push(full_path);
            _queue_cv.notify_one();
        }
        else if (PatternUtil::MatchesPattern(file_name, pattern)) {
            callback(directory_path, file_name, file_size, edit_timestamp, false);
        }
    } while (FindNextFileW(hFind, &find_data) != 0);

    FindClose(hFind);
#else
    DIR* dir = opendir(directory_path.c_str());
    if (!dir) return;

    struct dirent* entry;
    struct stat file_stat;
    wxString file_name;
    bool is_directory = false;

    while ((entry = readdir(dir)) != nullptr) {
        file_name = wxString(entry->d_name);
        if (file_name == "." || file_name == "..") continue;

        wxString full_path = directory_path + wxFileName::GetPathSeparator() + file_name;
        is_directory = (entry->d_type == DT_DIR);

        if (stat(full_path.c_str(), &file_stat) == -1) continue;

        uint64_t file_size = static_cast<uint64_t>(file_stat.st_size);
        time_t edit_timestamp = file_stat.st_mtime;

        if (is_directory) {
            if (PatternUtil::MatchesPattern(file_name, pattern)) {
                callback(directory_path, file_name, file_size, edit_timestamp, true);
            }

            std::lock_guard<std::mutex> lock(_queue_mutex);
            _directory_queue.push(full_path);
            _queue_cv.notify_one();
        }
        else if (PatternUtil::MatchesPattern(file_name, pattern)) {
            callback(directory_path, file_name, file_size, edit_timestamp, false);
        }
    }

    closedir(dir);
#endif
}

void FileProcessor::WorkerThread(const wxString& pattern,
    const SearchCallback& callback) {
    while (true) {
        wxString directory_path;

        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _queue_cv.wait(lock, [this] {
                return !_directory_queue.empty() || _stop_flag;
                });

            if (_stop_flag && _directory_queue.empty()) {
                break;
            }

            directory_path = _directory_queue.front();
            _directory_queue.pop();

            _active_threads++;
        }

        ProcessDirectory(directory_path, pattern, callback);

        {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            _active_threads--;
            if (_directory_queue.empty() && _active_threads == 0) {
                _stop_flag = true;
                _queue_cv.notify_all();
            }
        }
    }
}