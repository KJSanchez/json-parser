import { parse, tokenize } from "./json"

describe("lexer", () => {
    test.each([
        ['""', [""]],
        ['"string"', ["string"]],
        ['"1"', ["1"]],
        ["1", [1]],
        ["[]", ['"[", "]"']],
        ["[1]", []],
        ["[1, 2]", ["[", 1, ",", 2, "]"]],
        ["[1,2]", ["[", 1, ",", 2, "]"]],
        ["{}", ["{", "}"]],
        ['{"key": ""}', ["{", "key", ":", '""', "}"]],
        [
            '{"key": "", "key2": ""}',
            ["{", "key", ":", '""', ",", "key2", ":", '""', "}"],
        ],
    ])("It can tokenize JSON", (content, expected) => {
        expect(tokenize(content)).toStrictEqual(expected)
    })
})

describe("parser", () => {
    test.each([
        ['""'],
        ['"string"'],
        ['"1"'],
        ["1"],
        ["[]"],
        ["[1]"],
        ["[1, 2]"],
        ["[1,2]"],
        ["{}"],
        ['{"key": ""}'],
        ['{"key": "", "key2": ""}'],
    ])("It can parse JSON", (content) => {
        let expected = JSON.parse(content)
        expect(parse(content)).toStrictEqual(expected)
    })
})
