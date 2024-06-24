#define CATCH_CONFIG_MAIN
#include "json.cpp"
#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <vector>

using std::string, std::vector, std::pair, std::variant;

using token_tt = json::token_tt;

TEST_CASE("lexer") {
    vector<pair<string, json::tokens_t>> testcases = {
        {
            R"("")",
            {
                {token_tt::STRING, ""},
            },
        },
        {
            R"("string")",
            {
                {token_tt::STRING, "string"},
            },
        },
        {
            "1",
            {
                {token_tt::NUMBER, 1},
            },
        },
        {
            "true",
            {

                {token_tt::BOOLEAN, true},
            },
        },
        {
            "false",
            {

                {token_tt::BOOLEAN, false},
            },
        },
        {
            "null",
            {

                {token_tt::NULLPTR_T, nullptr},
            },
        },
        {
            "[]",
            {
                {token_tt::OPEN_BRACKET, "["},
                {token_tt::CLOSE_BRACKET, "]"},
            },
        },
        {
            "[1]",
            {
                {token_tt::OPEN_BRACKET, "["},
                {token_tt::NUMBER, 1},
                {token_tt::CLOSE_BRACKET, "]"},
            },
        },
        {
            "[1, 2]",
            {
                {token_tt::OPEN_BRACKET, "["},
                {token_tt::NUMBER, 1},
                {token_tt::COMMA, ","},
                {token_tt::NUMBER, 2},
                {token_tt::CLOSE_BRACKET, "]"},
            },
        },
        {
            "{}",
            {
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::CLOSE_BRACE, "}"},
            },
        },
        {
            R"({"key": "value"})",
            {
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::STRING, "key"},
                {token_tt::COLON, ":"},
                {token_tt::STRING, "value"},
                {token_tt::CLOSE_BRACE, "}"},
            },
        },
        {
            R"({"key": "value", "key2": "value2"})",
            {
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::STRING, "key"},
                {token_tt::COLON, ":"},
                {token_tt::STRING, "value"},
                {token_tt::COMMA, ","},
                {token_tt::STRING, "key2"},
                {token_tt::COLON, ":"},
                {token_tt::STRING, "value2"},
                {token_tt::CLOSE_BRACE, "}"},
            },
        },
        {

            R"({"key": {}})",
            {
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::STRING, "key"},
                {token_tt::COLON, ":"},
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::CLOSE_BRACE, "}"},
                {token_tt::CLOSE_BRACE, "}"},
            },
        },
        {
            R"({"key": {"key": "value"}})",
            {
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::STRING, "key"},
                {token_tt::COLON, ":"},
                {token_tt::OPEN_BRACE, "{"},
                {token_tt::STRING, "key"},
                {token_tt::COLON, ":"},
                {token_tt::STRING, "value"},
                {token_tt::CLOSE_BRACE, "}"},
                {token_tt::CLOSE_BRACE, "}"},
            },
        },
    };

    json::Lexer lexer;
    for (auto [testcase, expected] : testcases) {
        SECTION(testcase) {
            auto actual = lexer.tokenize(testcase);
            REQUIRE(actual == expected);
        }
    }
};

TEST_CASE("parser") {
    vector<string> testcases = {
        R"("")",
        "null",
        "1",
        "false",
        "true",
        R"("string")",
        "[]",
        "[1]",
        "[1, 2]",
        "[1, []]",
        "[1, [2, 3]]",
        "[1, [2, 3], 4]",
        "[1, [2, [3, 4]], 5]",
        "{}",
        R"({"key": "value"})",
        R"({"key": "value", "key2": "value2"})",
        R"({"key": {}})",
        R"({"key": {"key": "value"}})",
        R"({"key": {"key": {}}})",
    };

    json::Parser parser;
    for (auto testcase : testcases) {
        SECTION(testcase) {
            auto expected = nlohmann::json::parse(testcase);
            auto actual = parser.parse(testcase);
            REQUIRE(actual == expected);
        }
    }
}

TEST_CASE("variant") {
    variant<int, string> v = "foo", vv = "bar", vvv = 2;
    REQUIRE(std::get<string>(v) == "foo");

    REQUIRE(*std::get_if<string>(&v) == "foo");
    REQUIRE(v == variant<int, string>("foo"));
    REQUIRE_FALSE(v == vv);
    REQUIRE_FALSE(v == vvv);

    if (auto v = std::get_if<string>(&vv)) {
        REQUIRE(*v == "bar");
    } else if (std::get_if<int>(&vv)) {
        REQUIRE(false);
    }
}
