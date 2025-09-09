#pragma once
#include <vector>
#include <string>
#include <map>

struct Entry {
    size_t doc_id;
    size_t count;

    bool operator==(const Entry& other) const {
        return doc_id == other.doc_id && count == other.count;
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void UpdateDocumentBase(std::vector<std::string> input_docs);
    std::vector<Entry> GetWordCount(const std::string& word);
    size_t DocumentsCount() const { return docs.size(); }

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;

    void build_index_thread(size_t doc_id, const std::string& text);
    void merge_local_map(const std::map<std::string, size_t>& local, size_t doc_id);
};
