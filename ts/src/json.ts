export function parse(content: string): object {
    return JSON.parse(content)
}

enum TokenType {
    String,
    Number,
    ArrayStart,
    ArrayEnd,
    ObjectStart,
    ObjectEnd,
    Comma,
    Colon,
}

export function tokenize(content: string): object {
    let tokens = []
    for (const char of content) {
        if (char == '"') {
            tokens.push([TokenType.String, char])
        }
    }

    return tokens
}
