#include "SearchServer.h"
#include "utils.h"
#include <unordered_map>
#include <algorithm>

std::vector<std::vector<RelativeIndex>>
SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> all;

    for (auto &q_raw : queries_input) {
        auto words = split_words(to_lower_copy(q_raw));
        words = unique_words_preserve(words);       // не дублируем слова запроса
        if (words.empty()) { all.push_back({}); continue; }

        // 1) Абсолютная релевантность: суммируем count по всем словам запроса,
        //    по всем документам, где эти слова встречаются (логика OR).
        std::unordered_map<size_t, size_t> abs_rel; // doc_id -> sum_count
        for (const auto& w : words) {
            auto list = _index.GetWordCount(w);
            for (const auto& e : list) {
                abs_rel[e.doc_id] += e.count;      // добавляем частоту слова в документе
            }
        }

        if (abs_rel.empty()) {                      // по запросу ничего не найдено
            all.push_back({});
            continue;
        }

        // 2) Нормируем на максимум
        size_t max_abs = 0;
        for (const auto& p : abs_rel) max_abs = std::max(max_abs, p.second);

        std::vector<RelativeIndex> rel;
        rel.reserve(abs_rel.size());
        for (const auto& p : abs_rel) {
            float rank = (max_abs == 0) ? 0.f
                                        : static_cast<float>(p.second) / static_cast<float>(max_abs);
            rel.push_back(RelativeIndex{p.first, rank});
        }

        // 3) Сортировка: по рангу убыв., при равенстве — по doc_id возр.
        std::sort(rel.begin(), rel.end(), [](const RelativeIndex& a, const RelativeIndex& b){
            if (a.rank != b.rank) return a.rank > b.rank;
            return a.doc_id < b.doc_id;
        });

        // 4) Лимит
        if (_limit > 0 && rel.size() > static_cast<size_t>(_limit))
            rel.resize(_limit);

        all.push_back(std::move(rel));
    }

    return all;
}
