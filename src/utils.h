#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

inline std::string to_lower_copy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

inline std::vector<std::string> split_words(const std::string& text) {
    std::vector<std::string> out;
    std::string s = text;
 
    for (char& c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) c = ' ';
    }
    std::istringstream iss(s);
    std::string w;
    while (iss >> w) {
        out.push_back(w);
    }
    return out;
}

inline std::vector<std::string> unique_words_preserve(const std::vector<std::string>& words) {
    std::vector<std::string> uniq;
    std::unordered_set<std::string> seen;
    uniq.reserve(words.size());
    for (const auto& w : words) {
        if (!seen.count(w)) { seen.insert(w); uniq.push_back(w); }
    }
    return uniq;
}
