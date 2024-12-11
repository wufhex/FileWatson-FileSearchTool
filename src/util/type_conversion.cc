#include "util/type_conversion.hh"

uint64_t TypeConv::CombineU32(uint32_t hi, uint32_t lo) {
	return (static_cast<uint64_t>(hi) << 32) | lo;
}

wxString TypeConv::GetAppropriateSizeFormat(uint64_t bytes) {
    constexpr uint64_t KB = 1024;
    constexpr uint64_t MB = KB * 1024;
    constexpr uint64_t GB = MB * 1024;
    constexpr uint64_t TB = GB * 1024;

    if (bytes < KB) {
        return std::to_string(bytes) + " B";
    } else if (bytes < MB) {
        return std::to_string(bytes / KB) + " KB";
    } else if (bytes < GB) {
        return std::to_string(bytes / MB) + " MB";
    } else if (bytes < TB) {
        return std::to_string(bytes / GB) + " GB";
    } else {
        return std::to_string(bytes / TB) + " TB";
    }
}

#ifdef _WIN32
time_t TypeConv::Win_ToUnixTimestamp(ULONGLONG win_time) {
    constexpr uint64_t UNIX_EPOCH   = 11644473600ULL;
    constexpr uint64_t NANO_100_INT = 10000000ULL;

	return win_time / NANO_100_INT - UNIX_EPOCH;
}
#endif