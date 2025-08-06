#include "lexer.h"
#include <unordered_map>
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source) 
    : source(source), current(0), line(1), column(1), at_line_start(true) {
    indent_stack.push_back(0); // Start with no indentation
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        if (at_line_start) {
            auto indent_tokens = handleIndentation();
            tokens.insert(tokens.end(), indent_tokens.begin(), indent_tokens.end());
            at_line_start = false;
        }
        
        char c = advance();
        
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                // Skip whitespace (except at line start)
                break;
                
            case '\n':
                tokens.push_back(makeToken(TokenType::NEWLINE));
                line++;
                column = 1;
                at_line_start = true;
                break;
                
            case '#':
                // Skip comments
                while (peek() != '\n' && !isAtEnd()) advance();
                break;
                
            case '+':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::PLUS_ASSIGN, "+="));
                } else {
                    tokens.push_back(makeToken(TokenType::PLUS, "+"));
                }
                break;
                
            case '-':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::MINUS_ASSIGN, "-="));
                } else {
                    tokens.push_back(makeToken(TokenType::MINUS, "-"));
                }
                break;
                
            case '*':
                if (peek() == '*') {
                    advance();
                    tokens.push_back(makeToken(TokenType::POWER, "**"));
                } else {
                    tokens.push_back(makeToken(TokenType::MULTIPLY, "*"));
                }
                break;
                
            case '/':
                tokens.push_back(makeToken(TokenType::DIVIDE, "/"));
                break;
                
            case '%':
                tokens.push_back(makeToken(TokenType::MODULO, "%"));
                break;
                
            case '=':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::EQUAL, "=="));
                } else {
                    tokens.push_back(makeToken(TokenType::ASSIGN, "="));
                }
                break;
                
            case '!':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::NOT_EQUAL, "!="));
                } else {
                    tokens.push_back(makeToken(TokenType::INVALID, "!"));
                }
                break;
                
            case '<':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LESS_EQUAL, "<="));
                } else {
                    tokens.push_back(makeToken(TokenType::LESS, "<"));
                }
                break;
                
            case '>':
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::GREATER_EQUAL, ">="));
                } else {
                    tokens.push_back(makeToken(TokenType::GREATER, ">"));
                }
                break;
                
            case '(':
                tokens.push_back(makeToken(TokenType::LEFT_PAREN, "("));
                break;
                
            case ')':
                tokens.push_back(makeToken(TokenType::RIGHT_PAREN, ")"));
                break;
                
            case '[':
                tokens.push_back(makeToken(TokenType::LEFT_BRACKET, "["));
                break;
                
            case ']':
                tokens.push_back(makeToken(TokenType::RIGHT_BRACKET, "]"));
                break;
                
            case '{':
                tokens.push_back(makeToken(TokenType::LEFT_BRACE, "{"));
                break;
                
            case '}':
                tokens.push_back(makeToken(TokenType::RIGHT_BRACE, "}"));
                break;
                
            case ',':
                tokens.push_back(makeToken(TokenType::COMMA, ","));
                break;
                
            case '.':
                tokens.push_back(makeToken(TokenType::DOT, "."));
                break;
                
            case ':':
                tokens.push_back(makeToken(TokenType::COLON, ":"));
                break;
                
            case ';':
                tokens.push_back(makeToken(TokenType::SEMICOLON, ";"));
                break;
                
            case '"':
            case '\'':
                current--; // Back up to include quote
                tokens.push_back(string());
                break;
                
            default:
                if (std::isdigit(c)) {
                    current--; // Back up to include digit
                    tokens.push_back(number());
                } else if (std::isalpha(c) || c == '_') {
                    current--; // Back up to include first character
                    tokens.push_back(identifier());
                } else {
                    tokens.push_back(makeToken(TokenType::INVALID, std::string(1, c)));
                }
                break;
        }
    }
    
    // Handle final dedents
    while (indent_stack.size() > 1) {
        indent_stack.pop_back();
        tokens.push_back(makeToken(TokenType::DEDENT));
    }
    
    tokens.push_back(makeToken(TokenType::EOF_TOKEN));
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token(type, value, line, column);
}

Token Lexer::number() {
    std::string value;
    
    while (std::isdigit(peek())) {
        value += advance();
    }
    
    // Look for decimal point
    if (peek() == '.' && std::isdigit(peekNext())) {
        value += advance(); // Consume '.'
        while (std::isdigit(peek())) {
            value += advance();
        }
    }
    
    return makeToken(TokenType::NUMBER, value);
}

Token Lexer::string() {
    char quote = advance(); // Consume opening quote
    std::string value;
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        if (peek() == '\\') {
            advance(); // Consume backslash
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '\'': value += '\''; break;
                case '"': value += '"'; break;
                default: value += escaped; break;
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        return makeToken(TokenType::INVALID, "Unterminated string");
    }
    
    advance(); // Consume closing quote
    return makeToken(TokenType::STRING, value);
}

Token Lexer::identifier() {
    std::string value;
    
    while (std::isalnum(peek()) || peek() == '_') {
        value += advance();
    }
    
    TokenType type = identifierType(value);
    return makeToken(type, value);
}

TokenType Lexer::identifierType(const std::string& text) {
    static std::unordered_map<std::string, TokenType> keywords = {
        {"if", TokenType::IF},
        {"elif", TokenType::ELIF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"in", TokenType::IN},
        {"def", TokenType::DEF},
        {"return", TokenType::RETURN},
        {"class", TokenType::CLASS},
        {"import", TokenType::IMPORT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"try", TokenType::TRY},
        {"except", TokenType::EXCEPT},
        {"True", TokenType::TRUE},
        {"False", TokenType::FALSE},
        {"None", TokenType::NONE},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT}
    };
    
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return it->second;
    }
    
    return TokenType::IDENTIFIER;
}

std::vector<Token> Lexer::handleIndentation() {
    std::vector<Token> tokens;
    int spaces = 0;
    
    // Count leading spaces
    while (peek() == ' ') {
        spaces++;
        advance();
    }
    
    // Skip empty lines and comments
    if (peek() == '\n' || peek() == '#') {
        return tokens;
    }
    
    int current_indent = indent_stack.back();
    
    if (spaces > current_indent) {
        // Increased indentation
        indent_stack.push_back(spaces);
        tokens.push_back(makeToken(TokenType::INDENT));
    } else if (spaces < current_indent) {
        // Decreased indentation
        while (indent_stack.size() > 1 && indent_stack.back() > spaces) {
            indent_stack.pop_back();
            tokens.push_back(makeToken(TokenType::DEDENT));
        }
        
        if (indent_stack.back() != spaces) {
            tokens.push_back(makeToken(TokenType::INVALID, "Indentation error"));
        }
    }
    
    return tokens;
}
