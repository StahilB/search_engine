#pragma once
#include "InvertedIndex.h"
#include <vector>
#include <string>

struct RelativeIndex {
    size_t doc_id;
    float  rank;

    bool operator==(const RelativeIndex& other) const {
        return doc_id == other.doc_id && rank == other.rank;
    }
};

class SearchServer {
public:
    explicit SearchServer(InvertedIndex& idx, int responses_limit = 5)
        : _index(idx), _limit(responses_limit) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;
    int _limit;
};
