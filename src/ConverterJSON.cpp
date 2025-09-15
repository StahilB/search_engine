#ifndef PROJ_SOURCE_DIR
#define PROJ_SOURCE_DIR "."
#endif

#include "ConverterJSON.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
namespace fs = std::filesystem;
#include <map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

const char* ConverterJSON::kConfigPath   = PROJ_SOURCE_DIR "/configs/config.json";
const char* ConverterJSON::kRequestsPath = PROJ_SOURCE_DIR "/configs/requests.json";
const char* ConverterJSON::kAnswersPath  = PROJ_SOURCE_DIR "/configs/answers.json";

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::ifstream f(kConfigPath);
    if (!f.is_open()) {
        throw std::runtime_error("config file is missing");
    }

    json j; f >> j; f.close();
    if (!j.contains("config")) {
        throw std::runtime_error("config file is empty");
    }

    auto cfg = j.at("config");
    try {
        std::cout << "Starting " << cfg.at("name").get<std::string>()
                  << " v" << cfg.at("version").get<std::string>() << std::endl;
    } catch (...) {
        std::cout << "Starting SearchEngine" << std::endl;
    }

    std::vector<std::string> docs;
    if (j.contains("files") && j.at("files").is_array()) {
        for (auto& p : j.at("files")) {
            std::string rel = p.get<std::string>();
            fs::path path = rel;

            if (path.is_relative()) {
                path = fs::path(PROJ_SOURCE_DIR) / path;
            }

            std::ifstream in(path);
            if (!in.is_open()) {
                std::cerr << "ERROR: file not found: " << path.string() << std::endl;
                docs.emplace_back("");
                continue;
            }
            std::string content((std::istreambuf_iterator<char>(in)),
                                std::istreambuf_iterator<char>());
            in.close();
            docs.push_back(to_lower_copy(content));
        }
    }
    return docs;
}

int ConverterJSON::GetResponsesLimit() {
    std::ifstream f(kConfigPath);
    if (!f.is_open()) throw std::runtime_error("config file is missing");
    json j; f >> j; f.close();
    if (!j.contains("config")) throw std::runtime_error("config file is empty");
    auto cfg = j.at("config");
    if (cfg.contains("max_responses")) {
        return std::max(1, cfg.at("max_responses").get<int>());
    }
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::ifstream f(kRequestsPath);
    if (!f.is_open()) {
        return {};
    }
    json j; f >> j; f.close();
    std::vector<std::string> reqs;
    if (j.contains("requests") && j.at("requests").is_array()) {
        for (auto &q : j.at("requests")) {
            reqs.push_back(to_lower_copy(q.get<std::string>()));
        }
    }
    return reqs;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    json out;
    out["answers"] = json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        char idbuf[16];
        std::snprintf(idbuf, sizeof(idbuf), "request%03zu", i+1);
        auto &node = out["answers"][idbuf];

        if (answers[i].empty()) {
            node["result"] = "false";
        } else {
            node["result"] = "true";
            if (answers[i].size() > 1) {
                json rel = json::array();
                for (auto &p : answers[i]) {
                    rel.push_back({ {"docid", p.first}, {"rank", p.second} });
                }
                node["relevance"] = rel;
            } else {
                node["docid"] = answers[i][0].first;
                node["rank"]  = answers[i][0].second;
            }
        }
    }

    std::ofstream fo(kAnswersPath, std::ios::trunc);
    fo << out.dump(2);
    fo.close();
}
