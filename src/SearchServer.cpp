#include "SearchServer.h"
#include "utils.h"
#include <unordered_map>
#include <algorithm>

std::vector<std::vector<RelativeIndex>>
SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> all;

    for (auto &q_raw : queries_input) {
        auto words = split_words(to_lower_copy(q_raw));
        words = unique_words_preserve(words);
        if (words.empty()) { all.push_back({}); continue; }

        std::vector<std::pair<std::string, size_t>> word_freq;
        word_freq.reserve(words.size());
        for (auto &w : words) {
            auto vec = _index.GetWordCount(w);
            word_freq.push_back({w, vec.size()});
        }
        std::sort(word_freq.begin(), word_freq.end(),
                  [](auto &a, auto &b){ return a.second < b.second; });

        std::vector<size_t> current_docs;
        {
            auto first_list = _index.GetWordCount(word_freq.front().first);
            for (auto &e : first_list) current_docs.push_back(e.doc_id);
            std::sort(current_docs.begin(), current_docs.end());
            current_docs.erase(std::unique(current_docs.begin(), current_docs.end()), current_docs.end());
        }

        for (size_t i = 1; i < word_freq.size() && !current_docs.empty(); ++i) {
            auto next_list = _index.GetWordCount(word_freq[i].first);
            std::vector<size_t> next_docs;
            for (auto &e : next_list) next_docs.push_back(e.doc_id);
            std::sort(next_docs.begin(), next_docs.end());
            next_docs.erase(std::unique(next_docs.begin(), next_docs.end()), next_docs.end());

            std::vector<size_t> inter;
            std::set_intersection(current_docs.begin(), current_docs.end(),
                                  next_docs.begin(), next_docs.end(),
                                  std::back_inserter(inter));
            current_docs.swap(inter);
        }

        if (current_docs.empty()) {
            all.push_back({});
            continue;
        }

        std::unordered_map<size_t, size_t> abs_rel;
        for (auto &w : words) {
            auto vec = _index.GetWordCount(w);
            for (auto &e : vec) {
                if (std::find(current_docs.begin(), current_docs.end(), e.doc_id) != current_docs.end()) {
                    abs_rel[e.doc_id] += e.count;
                }
            }
        }

        size_t max_abs = 0;
        for (auto &p : abs_rel) if (p.second > max_abs) max_abs = p.second;

        std::vector<RelativeIndex> rel;
        rel.reserve(abs_rel.size());
        for (auto &p : abs_rel) {
            float rank = (max_abs == 0) ? 0.f : (static_cast<float>(p.second) / static_cast<float>(max_abs));
            rel.push_back(RelativeIndex{p.first, rank});
        }

        std::sort(rel.begin(), rel.end(), [](const RelativeIndex& a, const RelativeIndex& b){
            if (a.rank != b.rank) return a.rank > b.rank;
            return a.doc_id < b.doc_id;
        });

        if (_limit > 0 && rel.size() > static_cast<size_t>(_limit))
            rel.resize(_limit);

        all.push_back(rel);
    }

    return all;
}
