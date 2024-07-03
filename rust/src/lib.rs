#![allow(warnings)]
extern crate serde_json;

pub mod json {
    pub fn parse(content: &str) -> serde_json::Value {
        return serde_json::from_str(content).unwrap();
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


    pub fn tokenize(content: &str) -> Vec<Token> {
        let mut vec = Vec::new();

        let mut index = 0;

        while index < content.len() {

            if content.chars().nth(index).unwrap() == ' ' {
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

            if content.starts_with('"') {
                let mut string = "".to_string();
                while string.chars().filter(|&c| c == '"').count() != 2 {
                    string += &content.chars().nth(index).unwrap().to_string();
                    index += 1;
                }
                vec.push(Token::String(string));
                continue;
            }

            if content.starts_with("null") {
                vec.push(Token::Null(None));
                index += "null".len();
                continue;
            } else if content.starts_with("true") {
                vec.push(Token::Boolean(true));
            } else if content.starts_with("false") {
                vec.push(Token::Boolean(false));
            } else if content.starts_with("[") {
                vec.push(Token::LBracket("["));
            } else if content.starts_with("]") {
                vec.push(Token::RBracket("]"))
            } else if content.starts_with("{") {
                vec.push(Token::LBrace("{"))
            } else if content.starts_with("}") {
                vec.push(Token::RBrace("}"))
            } else if content.starts_with(":") {
                vec.push(Token::Colon(":"))
            } else if content.starts_with(",") {
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
                    let actual = crate::json::tokenize($input);
                    assert_eq!(actual, $expected);
                }
            )*
        }
    }

    lexer_tests! {
        lexer_00: r#""""#, vec![crate::json::Token::String("".to_string())],
        lexer_01: "null", vec![crate::json::Token::Null(None)],
        lexer_02: "1", vec![crate::json::Token::Number(1)],
    }

    macro_rules! parser_tests {
        ($($name:ident: $input:expr,)*) => {
            $(
                #[test]
                fn $name() {
                    let expected: serde_json::Value = serde_json::from_str($input).unwrap();
                    let actual = crate::json::parse($input);
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

        expected = serde_json::from_str("2").unwrap();
        assert_eq!(expected, 2);

        expected = serde_json::from_str("false").unwrap();
        assert_eq!(expected, false);

        expected = serde_json::from_str("[]").unwrap();
        let actual = serde_json::Value::Array(vec![]);
        assert_eq!(expected, actual);
    }

    #[test]
    fn grok_pattern_matching() {
        match 5 {
            1..=5 => assert_eq!(1, 1),
            _ => assert_eq!(1, 0),
        }
    }
}


fn dangle() -> String {
    let s = String::from("here");
    return s;
}

fn main() {
    let reference_to_string = dangle();
}
