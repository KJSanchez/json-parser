#![allow(warnings)]
extern crate serde_json;

pub mod json {
    pub fn parse(content: &str) -> serde_json::Value {
        return serde_json::from_str(content).unwrap();
    }
}


#[cfg(test)]
mod tests {
    #[test]
    fn test_parser() {
        let testcases = [
            "null",
            "1",
            "false",
            "true",
            r#""""#,
            r#""rust""#,
            "[]",
            "[1]",
            "[1, 2]",
            "[1, []]",
            "[1, [2, 3]]",
            "[1, [2, 3], 4]",
            "[1, [2, [3, 4]], 5]",
            "{}",
            r#"{"key": "value"}"#,
            r#"{"key": "value", "key2": "value2"}"#,
            r#"{"key": {}}"#,
            r#"{"key": {"key": "value"}}"#,
            r#"{"key": {"key": {}}}"#,
            r#"{"key": {"key": {"key": "value"}}}"#,
        ];

        for testcase in testcases.iter() {
            let expected: serde_json::Value = serde_json::from_str(testcase).unwrap();

            let actual = crate::json::parse(testcase);
            assert_eq!(actual, expected);
        }
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
