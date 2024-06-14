#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace json {

using std::string;
using std::variant;
using std::vector;
using json_t = nlohmann::json;

struct token_t {
    string type;
    variant<int, string, bool, std::nullptr_t> value;
};
using tokens_t = vector<token_t>;

bool operator==(const token_t &lhs, const token_t &rhs) {
    return lhs.type == rhs.type and lhs.value == rhs.value;
}

std::ostream &operator<<(std::ostream &os, const tokens_t &tokens) {
    os << "[";
    for (auto &token : tokens) {
        std::visit([&os](auto &&token) { os << token; }, token.value);
        if (&token != &tokens.back()) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

class ParseError : public std::runtime_error {
  public:
    ParseError(const string &message = "")
        : std::runtime_error(message) {
    }
};

class Lexer {
  public:
    tokens_t tokenize(string content) {
        tokens_t tokens;

        while (not content.empty()) {
            if (content[0] == '"') {
                string token = "";
                for (auto char_ : content) {
                    token.push_back(char_);
                    if (std::count(token.begin(), token.end(), '"') == 2) {
                        break;
                    }
                }
                tokens.push_back({
                    "string",
                    token.substr(1, token.length() - 2),
                });
                content = content.substr(token.length());
            } else if (isdigit(content[0])) {
                string token = "";
                for (auto char_ : content) {
                    if (isdigit(char_)) {
                        token.push_back(char_);
                    } else {
                        break;
                    }
                }
                tokens.push_back({"number", std::stoi(token)});
                content = content.substr(token.length());
            } else if (content.starts_with("null")) {
                tokens.push_back({"nullptr_t", nullptr});
                content = content.substr(4);
            } else if (content.starts_with("true")) {
                tokens.push_back({"boolean", true});
                content = content.substr(4);
            } else if (content.starts_with("false")) {
                tokens.push_back({"boolean", false});
                content = content.substr(5);
            } else if (content[0] == '[') {
                tokens.push_back({"[", "["});
                content = content.substr(1);
            } else if (content[0] == ']') {
                tokens.push_back({"]", "]"});
                content = content.substr(1);
            } else if (content[0] == '{') {
                tokens.push_back({"{", "{"});
                content = content.substr(1);
            } else if (content[0] == '}') {
                tokens.push_back({"}", "}"});
                content = content.substr(1);
            } else if (content[0] == ':') {
                tokens.push_back({":", ":"});
                content = content.substr(1);
            } else if (content[0] == ',') {
                tokens.push_back({",", ","});
                content = content.substr(1);
            } else if (content[0] == ' ') {
                content = content.substr(1);
            } else {
                std::cout << content << std::endl;
                throw ParseError();
            }
        }

        return tokens;
    }
};

class Parser {
    token_t current_token;
    tokens_t tokens;

    void advance() {
        tokens.erase(tokens.begin());
        current_token = tokens.front();
    }

    token_t expect(string token_type) {
        if (current_token.type != token_type) {
            throw ParseError();
        }

        auto current_token_copy = current_token;
        advance();
        return current_token_copy;
    }

    json_t array() {
        expect("[");

        auto json_array = json_t::array();
        while (current_token.type != "]") {
            if (current_token.type == "string") {
                json_array.push_back(std::get<string>(current_token.value));
                advance();
            } else if (current_token.type == "number") {
                json_array.push_back(std::get<int>(current_token.value));
                advance();
            } else if (current_token.type == "nullptr_t") {
                json_array.push_back(nullptr);
                advance();
            } else if (current_token.type == "boolean") {
                json_array.push_back(std::get<bool>(current_token.value));
                advance();
            } else if (current_token.type == "[") {
                json_array.push_back(array());
            } else if (current_token.type == "{") {
                json_array.push_back(object());
            } else {
                throw ParseError();
            }

            if (current_token.type != "]") {
                expect(",");
            }
        }

        expect("]");
        return json_array;
    }

    json_t object() {
        expect("{");
        auto json_object = json_t::object_t();

        while (current_token.type != "}") {
            string key = std::get<string>(expect("string").value);

            expect(":");

            if (current_token.type == "string") {
                json_object[key] = std::get<string>(current_token.value);
                advance();
            } else if (current_token.type == "number") {
                json_object[key] = std::get<int>(current_token.value);
                advance();
            } else if (current_token.type == "nullptr_t") {
                json_object[key] = nullptr;
                advance();
            } else if (current_token.type == ("boolean")) {
                json_object[key] = std::get<bool>(current_token.value);
                advance();
            } else if (current_token.type == "[") {
                json_object[key] = array();
            } else if (current_token.type == "{") {
                json_object[key] = object();
            } else {
                throw ParseError();
            }

            if (current_token.type != "}") {
                expect(",");
            }
        }

        expect("}");
        return json_object;
    }

  public:
    json_t parse(string content) {
        json_t json;
        tokens = Lexer().tokenize(content);
        current_token = tokens.front();
        while (not tokens.empty()) {
            if (current_token.type == "[") {
                json = array();
            } else if (current_token.type == "{") {
                json = object();
            } else if (current_token.type == "string") {
                json = std::get<string>(current_token.value);
                advance();
            } else if (current_token.type == "number") {
                json = std::get<int>(current_token.value);
                advance();
            } else if (current_token.type == "boolean") {
                json = std::get<bool>(current_token.value);
                advance();
            } else if (current_token.type == "nullptr_t") {
                json = nullptr;
                advance();
            } else {
                if (tokens.empty()) {
                    break;
                }
                throw ParseError();
            }
        }

        return json;
    }
};
} // namespace json
