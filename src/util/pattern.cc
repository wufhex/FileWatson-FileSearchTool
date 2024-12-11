#include "util/pattern.hh"

bool PatternUtil::MatchesPattern(const wxString& file_name, const wxString& pattern) {
    bool has_wildcards = pattern.find('*') != wxString::npos || pattern.find('?') != wxString::npos;

    if (has_wildcards) {
        wxString regex_pattern = "^";
        for (size_t i = 0; i < pattern.length(); ++i) {
            char c = pattern[i];
            if (c == '*') {
                regex_pattern += ".*";
            }
            else if (c == '?') {
                regex_pattern += ".";
            }
            else {
                regex_pattern += EscapeRegex(wxString(1, c));
            }
        }
        regex_pattern += "$";

        wxRegEx regex(regex_pattern, wxRE_ICASE);
        return regex.Matches(file_name);
    }

    wxString lower_filename = file_name.Lower();
    wxString lower_pattern = pattern.Lower();

    if (lower_filename.find(lower_pattern) != wxString::npos) {
        return true;
    }

    int max_distance = CalculateDynamicDistance(lower_filename, lower_pattern);
    return LevenshteinDistance(lower_filename, lower_pattern) <= max_distance;
}

wxString PatternUtil::EscapeRegex(const wxString& str) {
    static const std::unordered_set<char> special_chars = {
        '.', '^', '$', '(', ')', '[', ']', '{', '}', '|', '\\', '+', '*', '?'
    };

    wxString escaped;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (special_chars.count(c)) {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}

PatternUtil::SearchData PatternUtil::PathToSearchData(const wxString& path) {
    SearchData data;

    if (path.IsEmpty()) {
        data.valid = false;
        return data;
    }

    try {
        wxString filename = DirectoryUtil::GetLastSegment(path);

        data.valid = true;
        data.term = filename.IsEmpty() ? wxString("*.*") : filename;
        data.path = DirectoryUtil::RemovePathSpec(path);
    } catch (const std::exception& e) {
        data.valid = false;
    }

    if (data.path.IsEmpty()) {
        data.valid = false;
    }

    return data;
}

size_t PatternUtil::LevenshteinDistance(const wxString& s1, const wxString& s2) {
    const size_t len1 = s1.length(), len2 = s2.length();
    std::vector<std::vector<size_t>> d(len1 + 1, std::vector<size_t>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i) d[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) d[0][j] = j;

    for (size_t i = 1; i <= len1; ++i)
        for (size_t j = 1; j <= len2; ++j)
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1,
                                d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });

    return d[len1][len2];
}

int PatternUtil::CalculateDynamicDistance(const wxString& file_name, const wxString& pattern) {
    constexpr float DIFF = 0.25f;

    int max_length = static_cast<int>(std::max(file_name.length(), pattern.length()));
    int dynamic_distance = static_cast<int>(std::ceil(DIFF * max_length));

    return std::max(1, dynamic_distance);
}