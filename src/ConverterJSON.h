#pragma once
#include <vector>
#include <string>

class ConverterJSON {
public:
    ConverterJSON() = default;

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();

    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

    static const char* kConfigPath;
    static const char* kRequestsPath;
    static const char* kAnswersPath;
};
