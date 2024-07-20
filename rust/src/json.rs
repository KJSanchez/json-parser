#![allow(warnings)]
#![allow(dead_code)]
extern crate serde_json;

pub mod json {
    pub fn parse_object(tokens: Vec<Token>) -> serde_json::Value {
        let mut obj = serde_json::Map::new();
        let key = "key".to_string();
        obj[&key] = serde_json::Value::Bool(false);
        return serde_json::Value::Object(obj);
    }

    pub fn parse(content: &str) -> serde_json::Value {
        // return serde_json::from_str(&content).unwrap();

        let mut tokens = tokenize(&content);
        let next_token = &tokens;
        match &tokens[0] {
            Token::Null(_) => {
                return serde_json::Value::Null;
            }
            Token::Number(x) => {
                let val = serde_json::Number::from(*x);
                return serde_json::Value::Number(val);
            }
            Token::Boolean(x) => {
                return serde_json::Value::Bool(*x);
            }
            Token::String(x) => {
                return serde_json::Value::String(x.to_string());
            }
            // Token::LBrace()
            // Token::LBracket(_) => {
            //     return parse_array(tokens);
            // }
            _ => {
                panic!("not yet implemented");
            }
        }
        while !tokens.is_empty() {
            let front = tokens.remove(0);
            match &tokens[0] {
                Token::LBracket(_) => {}
                _ => {}
            }
        }
        return serde_json::from_str(&content).unwrap();
    }

    #[derive(Debug, PartialEq)]
    pub enum Token<'a> {
        Null(Option<u32>),
        Number(i32),
        String(String),
        Boolean(bool),
        LBracket(&'a str),
        RBracket(&'a str),
        LBrace(&'a str),
        RBrace(&'a str),
        Colon(&'a str),
        Comma(&'a str),
    }

    pub fn tokenize<'a>(content: &'a str) -> Vec<Token> {
        let mut vec = Vec::new();

        let mut index = 0;

        let content2 = &content;
        while index < content.len() {
            let content2: String = content2.chars().skip(index).collect();

            if content2 == "" {
                break;
            }

            if content2 == " " {
                index += 1;
                continue;
            }
            if content.chars().nth(index).unwrap().is_digit(10) {
                let mut number = "".to_string();
                while index < content.len() && content.chars().nth(index).unwrap().is_digit(10) {
                    number += &content.chars().nth(index).unwrap().to_string();
                    index += 1;
                }
                let number2 = number.parse::<i32>().unwrap();
                vec.push(Token::Number(number2));
                continue;
            }
            if content.chars().nth(index).unwrap() == '"' {
                let mut string = "".to_string();
                while string.chars().filter(|&c| c == '"').count() != 2 {
                    string += &content.chars().nth(index).unwrap().to_string();
                    index += 1;
                }

                string = string[1..(string.len() - 1)].to_string();
                vec.push(Token::String(string));
                continue;
            }

            if content2.starts_with("null") {
                vec.push(Token::Null(None));
                index += "null".len();
                continue;
            } else if content2.starts_with("true") {
                vec.push(Token::Boolean(true));
                index += "true".len();
                continue;
            } else if content2.starts_with("false") {
                vec.push(Token::Boolean(false));
                index += "false".len();
                continue;
            } else if content2.starts_with("[") {
                vec.push(Token::LBracket("["));
            } else if content2.starts_with("]") {
                vec.push(Token::RBracket("]"))
            } else if content2.starts_with("{") {
                vec.push(Token::LBrace("{"))
            } else if content2.starts_with("}") {
                vec.push(Token::RBrace("}"))
            } else if content2.starts_with(":") {
                vec.push(Token::Colon(":"))
            } else if content2.starts_with(",") {
                vec.push(Token::Comma(","));
            }

            index += 1;
        }

        return vec;
    }
}

#[cfg(test)]
mod tests {
    macro_rules! lexer_tests {
        ($($name:ident: $input:expr, $expected:expr,)*) => {
            $(
                #[test]
                fn $name() {
                    let actual = crate::json::json::tokenize($input);
                    assert_eq!(actual, $expected);
                }
            )*
        }
    }

    use crate::json::json::Token;
    lexer_tests! {
        lexer_00: r#""""#, vec![Token::String("".to_string())],
        lexer_01: "null", vec![Token::Null(None)],
        lexer_02: "1", vec![Token::Number(1)],
        lexer_03: "2", vec![Token::Number(2)],
        lexer_04: "true", vec![Token::Boolean(true)],
        lexer_05: "false", vec![Token::Boolean(false)],
        lexer_06: "[]", vec![Token::LBracket("["), Token::RBracket("]")],
        lexer_07: "[true, false]", vec![Token::LBracket("["), Token::Boolean(true), Token::Comma(","), Token::Boolean(false), Token::RBracket("]")],
        lexer_08: "{}", vec![Token::LBrace("{"), Token::RBrace("}")],
        lexer_09: r#"{"key": "value"}"#,
        vec![
            Token::LBrace("{"),
            Token::String("key".to_string()),
            Token::Colon(":"),
            Token::String("value".to_string()),
            Token::RBrace("}")
        ],
        lexer_10: r#"{"key": [true, [false]]}"#,
        vec![
            Token::LBrace("{"),
            Token::String("key".to_string()),
            Token::Colon(":"),
            Token::LBracket("["),
            Token::Boolean(true),
            Token::Comma(","),
            Token::LBracket("["),
            Token::Boolean(false),
            Token::RBracket("]"),
            Token::RBracket("]"),
            Token::RBrace("}")
        ],
    }

    macro_rules! parser_tests {
        ($($name:ident: $input:expr,)*) => {
            $(
                #[test]
                fn $name() {
                    let expected: serde_json::Value = serde_json::from_str($input).unwrap();
                    let actual = crate::json::json::parse($input);
                    assert_eq!(actual, expected);
                }
            )*
        }
    }

    parser_tests! {
        parser_00: "null",
        parser_01: "1",
        parser_02: "false",
        parser_03: "true",
        parser_04: r#""""#,
        parser_05: r#""rust""#,
        parser_06: "[]",
        parser_07: "[1]",
        parser_08: "[1, 2]",
        parser_09: "[1, []]",
        parser_10: "[1, [2, 3]]",
        parser_11: "[1, [2, 3], 4]",
        parser_12: "[1, [2, [3, 4]], 5]",
        parser_13: "{}",
        parser_14: r#"{"key": "value"}"#,
        parser_15: r#"{"key": "value", "key2": "value2"}"#,
        parser_16: r#"{"key": {}}"#,
        parser_17: r#"{"key": {"key": "value"}}"#,
        parser_18: r#"{"key": {"key": {}}}"#,
        parser_19: r#"{"key": {"key": {"key": "value"}}}"#,
    }

    #[test]
    fn grok_serde_json() {
        let mut expected: serde_json::Value = serde_json::from_str("1").unwrap();
        assert_eq!(expected, 1);

        // let vec = vec![1,2,3];
        // serde_json::json!(vec);

        expected = serde_json::from_str("2").unwrap();
        assert_eq!(expected, 2);

        expected = serde_json::from_str("false").unwrap();
        assert_eq!(expected, false);

        expected = serde_json::from_str("[]").unwrap();
        let actual = serde_json::Value::Array(vec![]);
        assert_eq!(expected, actual);
    }
}
