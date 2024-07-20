#![allow(warnings)]
#![allow(dead_code)]
extern crate serde_json;

#[cfg(test)]
mod tests {
    #[test]
    fn grok_pattern_matching() {
        match 4 {
            1..=5 => assert_eq!(1, 1),
            _ => assert_eq!(1, 1),
        }

        let content = "[]";

        match &content[..] {
            "[]" => {
                assert_eq!(true, true);
            }
            _ => {
                assert_eq!(true, false);
            }
        }

        assert_eq!(
            match true {
                true => true,
                _ => false,
            },
            true
        );
    }

    struct Foo {
        bar: i32,
    }

    fn push(foo: &mut Foo, item: i32) {
        foo.bar = item;
    }

    #[test]
    fn grok_borrowing() {
        let mut f = Foo { bar: 10 };
        {
            let mut foo = Foo { bar: 3 };
            assert_eq!(foo.bar, 3);
            push(&mut foo, 4);
        }
        assert_eq!(f.bar, 10);
    }

    // This says that any reference to Bar cannot outlive the reference an i32 it contains.
    struct Bar<'a> {
        x: &'a i32,
    }

    #[test]
    fn grok_lifetimes() {
        let x;
        let f;

        {
            let y = &5;
            f = Bar { x: y };
            x = f.x;
        }

        assert_eq!(*x, 5);
    }

    #[test]
    fn grok_pointers() {
        let mut x = 5;
        let y = &x; // `y` is a reference to `x`
        assert_eq!(*y, 5);
    }

    macro_rules! grok_macros_test {
        ($($name:ident: $input:expr,)*) => {
            $(
                #[test]
                fn $name() {
                    assert_eq!($input, $input);
                }
            )*
        }
    }

    grok_macros_test! {
        test0: "foo",
        test1: "bar",
    }
}
