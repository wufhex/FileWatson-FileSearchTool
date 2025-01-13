#pragma once
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/filefn.h>
#include <wx/filename.h>

#include <unordered_set>
#include <string>
#include <filesystem>
#include <regex>
#include <cmath>

#include "dir_util.hh"
#include "type_conversion.hh"

#ifdef _WIN32
#undef min
#undef max
#endif

namespace fs = std::filesystem;

class PatternUtil {
public:
	typedef struct SearchData {
		wxString path;
		wxString term;
	} SearchData;

	static bool        MatchesPattern(const wxString& file_name, const wxString& pattern);
private:
	static wxString    EscapeRegex(const wxString& str);
	static size_t	   LevenshteinDistance(const wxString& s1, const wxString& s2);
	static int         CalculateDynamicDistance(const wxString& file_name, const wxString& pattern);
};