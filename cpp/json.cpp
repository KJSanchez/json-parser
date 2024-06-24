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

enum token_tt {
    STRING,
    NUMBER,
    NULLPTR_T,
    BOOLEAN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_BRACE,
    CLOSE_BRACE,
    COLON,
    COMMA,
};

struct token_t {
    token_tt type;
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
                    token_tt::STRING,
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
                tokens.push_back({token_tt::NUMBER, std::stoi(token)});
                content = content.substr(token.length());
            } else if (content.starts_with("null")) {
                tokens.push_back({token_tt::NULLPTR_T, nullptr});
                content = content.substr(4);
            } else if (content.starts_with("true")) {
                tokens.push_back({token_tt::BOOLEAN, true});
                content = content.substr(4);
            } else if (content.starts_with("false")) {
                tokens.push_back({token_tt::BOOLEAN, false});
                content = content.substr(5);
            } else if (content[0] == '[') {
                tokens.push_back({token_tt::OPEN_BRACKET, "["});
                content = content.substr(1);
            } else if (content[0] == ']') {
                tokens.push_back({token_tt::CLOSE_BRACKET, "]"});
                content = content.substr(1);
            } else if (content[0] == '{') {
                tokens.push_back({token_tt::OPEN_BRACE, "{"});
                content = content.substr(1);
            } else if (content[0] == '}') {
                tokens.push_back({token_tt::CLOSE_BRACE, "}"});
                content = content.substr(1);
            } else if (content[0] == ':') {
                tokens.push_back({token_tt::COLON, ":"});
                content = content.substr(1);
            } else if (content[0] == ',') {
                tokens.push_back({token_tt::COMMA, ","});
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

    token_t expect(token_tt token_type) {
        if (current_token.type != token_type) {
            throw ParseError();
        }

        auto current_token_copy = current_token;
        advance();
        return current_token_copy;
    }

    json_t array() {
        expect(token_tt::OPEN_BRACKET);

        auto json_array = json_t::array();
        while (current_token.type != token_tt::CLOSE_BRACKET) {
            if (current_token.type == token_tt::STRING) {
                json_array.push_back(std::get<string>(current_token.value));
                advance();
            } else if (current_token.type == token_tt::NUMBER) {
                json_array.push_back(std::get<int>(current_token.value));
                advance();
            } else if (current_token.type == token_tt::NULLPTR_T) {
                json_array.push_back(nullptr);
                advance();
            } else if (current_token.type == token_tt::BOOLEAN) {
                json_array.push_back(std::get<bool>(current_token.value));
                advance();
            } else if (current_token.type == token_tt::OPEN_BRACKET) {
                json_array.push_back(array());
            } else if (current_token.type == token_tt::OPEN_BRACE) {
                json_array.push_back(object());
            } else {
                throw ParseError();
            }

            if (current_token.type != token_tt::CLOSE_BRACKET) {
                expect(token_tt::COMMA);
            }
        }

        expect(token_tt::CLOSE_BRACKET);
        return json_array;
    }

    json_t object() {
        expect(token_tt::OPEN_BRACE);
        auto json_object = json_t::object_t();

        while (current_token.type != token_tt::CLOSE_BRACE) {
            string key = std::get<string>(expect(token_tt::STRING).value);

            expect(token_tt::COLON);

            if (current_token.type == token_tt::STRING) {
                json_object[key] = std::get<string>(current_token.value);
                advance();
            } else if (current_token.type == token_tt::NUMBER) {
                json_object[key] = std::get<int>(current_token.value);
                advance();
            } else if (current_token.type == token_tt::NULLPTR_T) {
                json_object[key] = nullptr;
                advance();
            } else if (current_token.type == token_tt::BOOLEAN) {
                json_object[key] = std::get<bool>(current_token.value);
                advance();
            } else if (current_token.type == token_tt::OPEN_BRACKET) {
                json_object[key] = array();
            } else if (current_token.type == token_tt::OPEN_BRACE) {
                json_object[key] = object();
            } else {
                throw ParseError();
            }

            if (current_token.type != token_tt::CLOSE_BRACE) {
                expect(token_tt::COMMA);
            }
        }

        expect(token_tt::CLOSE_BRACE);
        return json_object;
    }

  public:
    json_t parse(string content) {
        json_t json;
        tokens = Lexer().tokenize(content);
        current_token = tokens.front();
        while (not tokens.empty()) {
            if (current_token.type == token_tt::OPEN_BRACKET) {
                json = array();
            } else if (current_token.type == token_tt::OPEN_BRACE) {
                json = object();
            } else if (current_token.type == token_tt::STRING) {
                json = std::get<string>(current_token.value);
                advance();
            } else if (current_token.type == token_tt::NUMBER) {
                json = std::get<int>(current_token.value);
                advance();
            } else if (current_token.type == token_tt::BOOLEAN) {
                json = std::get<bool>(current_token.value);
                advance();
            } else if (current_token.type == token_tt::NULLPTR_T) {
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
