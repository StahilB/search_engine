#include <gtest/gtest.h>
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <cmath>

using namespace std;

static void TestInvertedIndexFunctionality(
    const vector<string>& docs,
    const vector<string>& requests,
    const vector<vector<Entry>>& expected
) {
    vector<vector<Entry>> result;
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);

    for (auto &request : requests) {
        auto wc = idx.GetWordCount(request);
        result.push_back(wc);
    }
    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
    const vector<string> docs = {
        "london is the capital of great britain",
        "big ben is the nickname for the great bell of the striking clock"
    };
    const vector<string> requests = {"london", "the"};
    const vector<vector<Entry>> expected = {
        { {0,1} },
        { {0,1}, {1,3} }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestBasic2) {
    const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const vector<string> requests = {"milk", "water", "cappuccino"};
    const vector<vector<Entry>> expected = {
        { {0,4}, {1,1}, {2,5} },
        { {0,3}, {1,2}, {2,5} },
        { {3,1} }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestMissing) {
    const vector<string> docs = {
        "a  b  c  d  e  f  g  h  i  j  k  l",
        "statement"
    };
    const vector<string> requests = {"m", "statement"};
    const vector<vector<Entry>> expected = {
        { },
        { {1,1} }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseSearchServer, TestSimple) {
    const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const vector<string> request = {"milk water", "sugar"};
    const vector<vector<RelativeIndex>> expected = {
        { {2,1.0f}, {0,0.7f}, {1,0.3f} },
        { }
    };
    InvertedIndex idx; idx.UpdateDocumentBase(docs);
    SearchServer srv(idx, 5);
    auto result = srv.search(request);
    ASSERT_EQ(result.size(), expected.size());

    auto cmp = [](const vector<RelativeIndex>& a, const vector<RelativeIndex>& b){
        if (a.size()!=b.size()) return false;
        for (size_t i=0;i<a.size();++i){
            if (a[i].doc_id != b[i].doc_id) return false;
            if (std::abs(a[i].rank - b[i].rank) > 1e-6f) return false;
        }
        return true;
    };
    EXPECT_TRUE(cmp(result[0], expected[0]));
    EXPECT_TRUE(cmp(result[1], expected[1]));
}

TEST(TestCaseSearchServer, TestTop5) {
    const vector<string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockholm is the capital of sweden",
        "riga is the capital of latvia",
        "tallinn is the capital of estonia",
        "warsaw is the capital of poland"
    };

    const vector<string> request = {"moscow is the capital of russia"};
    InvertedIndex idx; idx.UpdateDocumentBase(docs);
    SearchServer srv(idx, 5);
    auto result = srv.search(request);

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0][0].doc_id, 7u);
    EXPECT_NEAR(result[0][0].rank, 1.0f, 1e-6f);
    EXPECT_EQ(result[0][1].doc_id, 14u);
    EXPECT_NEAR(result[0][1].rank, 1.0f, 1e-6f);
}
