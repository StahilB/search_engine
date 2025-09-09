#include "InvertedIndex.h"
#include "utils.h"
#include <thread>
#include <mutex>
#include <algorithm>
#include <map>

namespace {
std::mutex g_merge_mutex;
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = std::move(input_docs);
    freq_dictionary.clear();

    std::vector<std::thread> threads;
    threads.reserve(docs.size());
    for (size_t i = 0; i < docs.size(); ++i) {
        threads.emplace_back([this, i]{
            build_index_thread(i, docs[i]);
        });
    }
    for (auto &t : threads) t.join();

    for (auto &kv : freq_dictionary) {
        auto &vec = kv.second;
        std::sort(vec.begin(), vec.end(), [](const Entry& a, const Entry& b){
            return a.doc_id < b.doc_id;
        });
    }
}

void InvertedIndex::build_index_thread(size_t doc_id, const std::string& text) {
    std::map<std::string, size_t> local;
    for (const auto &w : split_words(text)) {
        local[w] += 1;
    }
    merge_local_map(local, doc_id);
}

void InvertedIndex::merge_local_map(const std::map<std::string, size_t>& local, size_t doc_id) {
    std::lock_guard<std::mutex> lk(g_merge_mutex);
    for (const auto &kv : local) {
        auto &vec = freq_dictionary[kv.first];
        vec.push_back(Entry{doc_id, kv.second});
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    auto it = freq_dictionary.find(word);
    if (it == freq_dictionary.end()) return {};
    return it->second;
}
