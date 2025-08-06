#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

std::unique_ptr<Program> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!isAtEnd()) {
        // Skip newlines at top level
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        try {
            auto stmt = statement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return std::make_unique<Program>(std::move(statements));
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    
    throw std::runtime_error(message + " at line " + std::to_string(peek().line));
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) return;
        
        switch (peek().type) {
            case TokenType::DEF:
            case TokenType::CLASS:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<Statement> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::DEF})) return functionDefStatement();
    if (match({TokenType::CLASS})) return classDefStatement();
    if (match({TokenType::IMPORT})) return importStatement();
    if (match({TokenType::FROM})) return fromImportStatement();
    if (match({TokenType::TRY})) return tryStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    
    // Check for assignment (both simple and attribute)
    if (check(TokenType::IDENTIFIER)) {
        size_t saved = current;
        advance(); // consume identifier
        
        if (check(TokenType::ASSIGN)) {
            // Simple assignment: identifier = value
            current = saved; // reset
            return assignmentStatement();
        } else if (check(TokenType::DOT)) {
            advance(); // consume dot
            if (check(TokenType::IDENTIFIER)) {
                advance(); // consume attribute
                if (check(TokenType::ASSIGN)) {
                    // Attribute assignment: obj.attr = value
                    current = saved; // reset
                    return attributeAssignmentStatement();
                }
            }
        }
        
        current = saved; // reset
    }
    
    return expressionStatement();
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr = expression();
    
    // Consume optional newline
    if (check(TokenType::NEWLINE)) {
        advance();
    }
    
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::assignmentStatement() {
    Token name = advance(); // consume identifier
    consume(TokenType::ASSIGN, "Expected '=' after variable name");
    
    auto value = expression();
    
    // Consume optional newline
    if (check(TokenType::NEWLINE)) {
        advance();
    }
    
    return std::make_unique<AssignmentStatement>(name.value, std::move(value));
}

std::unique_ptr<Statement> Parser::attributeAssignmentStatement() {
    auto object = std::make_unique<IdentifierExpression>(advance().value); // consume object identifier
    consume(TokenType::DOT, "Expected '.' after object");
    
    if (!check(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected attribute name after '.'");
    }
    Token attribute = advance(); // consume attribute
    
    consume(TokenType::ASSIGN, "Expected '=' after attribute name");
    
    auto value = expression();
    
    // Consume optional newline
    if (check(TokenType::NEWLINE)) {
        advance();
    }
    
    return std::make_unique<AttributeAssignmentStatement>(std::move(object), attribute.value, std::move(value));
}

std::unique_ptr<Statement> Parser::ifStatement() {
    auto condition = expression();
    consume(TokenType::COLON, "Expected ':' after if condition");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation after if statement");
    
    auto then_branch = blockStatement();
    
    std::unique_ptr<Statement> else_branch = nullptr;
    if (match({TokenType::ELSE})) {
        consume(TokenType::COLON, "Expected ':' after else");
        consume(TokenType::NEWLINE, "Expected newline after ':'");
        consume(TokenType::INDENT, "Expected indentation after else statement");
        else_branch = blockStatement();
    }
    
    return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Statement> Parser::whileStatement() {
    auto condition = expression();
    consume(TokenType::COLON, "Expected ':' after while condition");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation after while statement");
    
    auto body = blockStatement();
    
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::forStatement() {
    // Expect: for <variable> in <iterable>:
    consume(TokenType::IDENTIFIER, "Expected variable name after 'for'");
    std::string variable = previous().value;
    
    consume(TokenType::IN, "Expected 'in' after for variable");
    auto iterable = expression();
    
    consume(TokenType::COLON, "Expected ':' after for clause");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation after for statement");
    
    auto body = blockStatement();
    
    return std::make_unique<ForStatement>(variable, std::move(iterable), std::move(body));
}

std::unique_ptr<Statement> Parser::functionDefStatement() {
    Token name = advance();
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    std::vector<std::string> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            consume(TokenType::IDENTIFIER, "Expected parameter name");
            parameters.push_back(previous().value);
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    consume(TokenType::COLON, "Expected ':' after function signature");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation after function definition");
    
    auto body = blockStatement();
    
    return std::make_unique<FunctionDefStatement>(name.value, std::move(parameters), std::move(body));
}

std::unique_ptr<Statement> Parser::classDefStatement() {
    Token name = advance();
    consume(TokenType::COLON, "Expected ':' after class name");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation after class definition");
    
    auto body = blockStatement();
    
    return std::make_unique<ClassDefStatement>(name.value, std::move(body));
}

std::unique_ptr<Statement> Parser::importStatement() {
    // import module_name [as alias]
    if (!check(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected module name after 'import'");
    }
    Token module_name = advance();
    
    std::string alias = "";
    if (match({TokenType::AS})) {
        if (!check(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected alias name after 'as'");
        }
        alias = advance().value;
    }
    
    // Consume optional newline
    if (check(TokenType::NEWLINE)) {
        advance();
    }
    
    return std::make_unique<ImportStatement>(module_name.value, alias);
}

std::unique_ptr<Statement> Parser::fromImportStatement() {
    // from module_name import name1 [as alias1], name2 [as alias2], ...
    if (!check(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected module name after 'from'");
    }
    Token module_name = advance();
    
    consume(TokenType::IMPORT, "Expected 'import' after module name");
    
    std::vector<std::pair<std::string, std::string>> imports;
    
    do {
        if (!check(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected import name");
        }
        Token import_name = advance();
        
        std::string alias = "";
        if (match({TokenType::AS})) {
            if (!check(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected alias name after 'as'");
            }
            alias = advance().value;
        }
        
        imports.push_back({import_name.value, alias});
    } while (match({TokenType::COMMA}));
    
    // Consume optional newline
    if (check(TokenType::NEWLINE)) {
        advance();
    }
    
    return std::make_unique<FromImportStatement>(module_name.value, std::move(imports));
}

std::unique_ptr<Statement> Parser::returnStatement() {
    std::unique_ptr<Expression> value = nullptr;
    
    if (!check(TokenType::NEWLINE) && !isAtEnd()) {
        value = expression();
    }
    
    // Consume optional newline
    if (check(TokenType::NEWLINE)) {
        advance();
    }
    
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Statement> Parser::tryStatement() {
    // Parse try block
    consume(TokenType::COLON, "Expected ':' after 'try'");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation after try statement");
    auto try_body = blockStatement();
    
    // Parse except clauses
    std::vector<ExceptClause> except_clauses;
    
    while (match({TokenType::EXCEPT})) {
        std::string exception_type = "";
        std::string variable_name = "";
        
        // Check if there's an exception type specified
        if (check(TokenType::IDENTIFIER)) {
            exception_type = advance().value;
            
            // Check if there's a variable binding (as variable)
            if (match({TokenType::AS})) {
                if (!check(TokenType::IDENTIFIER)) {
                    throw std::runtime_error("Expected variable name after 'as'");
                }
                variable_name = advance().value;
            }
        }
        
        consume(TokenType::COLON, "Expected ':' after except clause");
        consume(TokenType::NEWLINE, "Expected newline after ':'");
        consume(TokenType::INDENT, "Expected indentation after except clause");
        auto except_body = blockStatement();
        
        except_clauses.emplace_back(exception_type, variable_name, std::move(except_body));
    }
    
    if (except_clauses.empty()) {
        throw std::runtime_error("Try statement must have at least one except clause");
    }
    
    return std::make_unique<TryStatement>(std::move(try_body), std::move(except_clauses));
}

std::unique_ptr<BlockStatement> Parser::blockStatement() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!check(TokenType::DEDENT) && !isAtEnd()) {
        // Skip newlines in blocks
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        auto stmt = statement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    consume(TokenType::DEDENT, "Expected dedent to close block");
    
    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Expression> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match({TokenType::OR})) {
        TokenType operator_type = previous().type;
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match({TokenType::AND})) {
        TokenType operator_type = previous().type;
        auto right = equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::NOT_EQUAL, TokenType::EQUAL})) {
        TokenType operator_type = previous().type;
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = term();
    
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        TokenType operator_type = previous().type;
        auto right = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr = factor();
    
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        TokenType operator_type = previous().type;
        auto right = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr = power();
    
    while (match({TokenType::DIVIDE, TokenType::MULTIPLY, TokenType::MODULO})) {
        TokenType operator_type = previous().type;
        auto right = power();
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::power() {
    auto expr = unary();
    
    if (match({TokenType::POWER})) {
        TokenType operator_type = previous().type;
        auto right = power(); // Right associative
        expr = std::make_unique<BinaryExpression>(std::move(expr), operator_type, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        TokenType operator_type = previous().type;
        auto right = unary();
        return std::make_unique<UnaryExpression>(operator_type, std::move(right));
    }
    
    return call();
}

std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            auto args = arguments();
            consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");
            expr = std::make_unique<CallExpression>(std::move(expr), std::move(args));
        } else if (match({TokenType::LEFT_BRACKET})) {
            auto index = expression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after index");
            expr = std::make_unique<IndexExpression>(std::move(expr), std::move(index));
        } else if (match({TokenType::DOT})) {
            if (!check(TokenType::IDENTIFIER)) {
                throw std::runtime_error("Expected attribute name after '.'");
            }
            Token name = advance();
            expr = std::make_unique<AttributeExpression>(std::move(expr), name.value);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match({TokenType::TRUE})) {
        return std::make_unique<BooleanExpression>(true);
    }
    
    if (match({TokenType::FALSE})) {
        return std::make_unique<BooleanExpression>(false);
    }
    
    if (match({TokenType::NONE})) {
        return std::make_unique<NoneExpression>();
    }
    
    if (match({TokenType::NUMBER})) {
        double value = std::stod(previous().value);
        return std::make_unique<NumberExpression>(value);
    }
    
    if (match({TokenType::STRING})) {
        return std::make_unique<StringExpression>(previous().value);
    }
    
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpression>(previous().value);
    }
    
    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    if (match({TokenType::LEFT_BRACKET})) {
        // Parse list literal
        std::vector<std::unique_ptr<Expression>> elements;
        
        if (!check(TokenType::RIGHT_BRACKET)) {
            do {
                elements.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after list elements");
        return std::make_unique<ListExpression>(std::move(elements));
    }
    
    if (match({TokenType::LEFT_BRACE})) {
        // Parse dictionary literal
        std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> pairs;
        
        if (!check(TokenType::RIGHT_BRACE)) {
            do {
                auto key = expression();
                consume(TokenType::COLON, "Expected ':' after dictionary key");
                auto value = expression();
                pairs.emplace_back(std::move(key), std::move(value));
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RIGHT_BRACE, "Expected '}' after dictionary pairs");
        return std::make_unique<DictExpression>(std::move(pairs));
    }
    
    throw std::runtime_error("Expected expression at line " + std::to_string(peek().line));
}

std::vector<std::unique_ptr<Expression>> Parser::arguments() {
    std::vector<std::unique_ptr<Expression>> args;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    
    return args;
}
