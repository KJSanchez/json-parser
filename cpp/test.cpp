#define CATCH_CONFIG_MAIN
#include "json.cpp"
#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <vector>

using std::string, std::vector, std::pair, std::variant;

TEST_CASE("lexer") {
    vector<pair<string, json::tokens_t>> testcases = {
        {
            R"("")",
            {
                {"string", ""},
            },
        },
        {
            R"("string")",
            {
                {"string", "string"},
            },
        },
        {
            "1",
            {
                {"number", 1},
            },
        },
        {
            "true",
            {

                {"boolean", true},
            },
        },
        {
            "false",
            {

                {"boolean", false},
            },
        },
        {
            "null",
            {

                {"nullptr_t", nullptr},
            },
        },
        {
            "[]",
            {
                {"[", "["},
                {"]", "]"},
            },
        },
        {
            "[1]",
            {
                {"[", "["},
                {"number", 1},
                {"]", "]"},
            },
        },
        {
            "[1, 2]",
            {
                {"[", "["},
                {"number", 1},
                {",", ","},
                {"number", 2},
                {"]", "]"},
            },
        },
        {
            "{}",
            {
                {"{", "{"},
                {"}", "}"},
            },
        },
        {
            R"({"key": "value"})",
            {
                {"{", "{"},
                {"string", "key"},
                {":", ":"},
                {"string", "value"},
                {"}", "}"},
            },
        },
        {
            R"({"key": "value", "key2": "value2"})",
            {
                {"{", "{"},
                {"string", "key"},
                {":", ":"},
                {"string", "value"},
                {",", ","},
                {"string", "key2"},
                {":", ":"},
                {"string", "value2"},
                {"}", "}"},
            },
        },
        {

            R"({"key": {}})",
            {
                {"{", "{"},
                {"string", "key"},
                {":", ":"},
                {"{", "{"},
                {"}", "}"},
                {"}", "}"},
            },
        },
        {
            R"({"key": {"key": "value"}})",
            {
                {"{", "{"},
                {"string", "key"},
                {":", ":"},
                {"{", "{"},
                {"string", "key"},
                {":", ":"},
                {"string", "value"},
                {"}", "}"},
                {"}", "}"},
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
