#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <iostream>

int main() {
    try {
        ConverterJSON cj;

        auto docs = cj.GetTextDocuments();
        InvertedIndex index;
        index.UpdateDocumentBase(docs);

        auto queries = cj.GetRequests();
        int limit = cj.GetResponsesLimit();
        SearchServer server(index, limit);
        auto result = server.search(queries);

        std::vector<std::vector<std::pair<int, float>>> to_json;
        to_json.reserve(result.size());
        for (auto &vec : result) {
            std::vector<std::pair<int,float>> one;
            for (auto &ri : vec) {
                one.emplace_back(static_cast<int>(ri.doc_id), ri.rank);
            }
            to_json.push_back(std::move(one));
        }

        cj.putAnswers(to_json);
        std::cout << "Done. See configs/answers.json\n";
    } catch (const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
