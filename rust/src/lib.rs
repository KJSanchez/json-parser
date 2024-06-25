pub mod json {

    pub fn parse() -> i32 {
        return 1;
    }
}



#[cfg(test)]
mod tests {
    #[test]
    fn test_1() {
        let a = crate::json::parse();
        assert_eq!(a, 1);
    }
}
