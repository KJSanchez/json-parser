#![allow(warnings)]
extern crate serde_json;

pub mod json {
    pub fn parse(content: &str) -> serde_json::Value {
        return serde_json::from_str(content).unwrap();
    }
}


#[cfg(test)]
mod tests {
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
    fn test_grok_serde_json() {
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
}
