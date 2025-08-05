#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Literals
    NUMBER,
    STRING,
    IDENTIFIER,
    
    // Keywords
    IF,
    ELIF,
    ELSE,
    WHILE,
    FOR,
    IN,
    DEF,
    RETURN,
    CLASS,
    IMPORT,
    FROM,
    AS,
    TRUE,
    FALSE,
    NONE,
    AND,
    OR,
    NOT,
    
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    POWER,
    ASSIGN,
    PLUS_ASSIGN,
    MINUS_ASSIGN,
    
    // Comparison
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    
    // Delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    COLON,
    SEMICOLON,
    
    // Special
    NEWLINE,
    INDENT,
    DEDENT,
    EOF_TOKEN,
    
    // Invalid
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t current;
    int line;
    int column;
    std::vector<int> indent_stack;
    bool at_line_start;
    
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& value = "");
    Token number();
    Token string();
    Token identifier();
    TokenType identifierType(const std::string& text);
    std::vector<Token> handleIndentation();
};
