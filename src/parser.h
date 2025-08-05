#pragma once
#include "lexer.h"
#include <memory>
#include <vector>

// Forward declarations
struct ASTNode;
struct Expression;
struct Statement;

// AST Node types
enum class NodeType {
    // Expressions
    NUMBER_EXPR,
    STRING_EXPR,
    BOOLEAN_EXPR,
    IDENTIFIER_EXPR,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    
    // Statements
    EXPRESSION_STMT,
    ASSIGNMENT_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    FUNCTION_DEF_STMT,
    RETURN_STMT,
    BLOCK_STMT,
    
    // Program
    PROGRAM
};

// Base AST Node
struct ASTNode {
    NodeType type;
    int line;
    int column;
    
    ASTNode(NodeType t, int l = 0, int c = 0) : type(t), line(l), column(c) {}
    virtual ~ASTNode() = default;
};

// Expression nodes
struct Expression : public ASTNode {
    Expression(NodeType t, int l = 0, int c = 0) : ASTNode(t, l, c) {}
};

struct NumberExpression : public Expression {
    double value;
    NumberExpression(double v, int l = 0, int c = 0) 
        : Expression(NodeType::NUMBER_EXPR, l, c), value(v) {}
};

struct StringExpression : public Expression {
    std::string value;
    StringExpression(const std::string& v, int l = 0, int c = 0)
        : Expression(NodeType::STRING_EXPR, l, c), value(v) {}
};

struct BooleanExpression : public Expression {
    bool value;
    BooleanExpression(bool v, int l = 0, int c = 0)
        : Expression(NodeType::BOOLEAN_EXPR, l, c), value(v) {}
};

struct IdentifierExpression : public Expression {
    std::string name;
    IdentifierExpression(const std::string& n, int l = 0, int c = 0)
        : Expression(NodeType::IDENTIFIER_EXPR, l, c), name(n) {}
};

struct BinaryExpression : public Expression {
    std::unique_ptr<Expression> left;
    TokenType operator_type;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(std::unique_ptr<Expression> l, TokenType op, std::unique_ptr<Expression> r, int line = 0, int col = 0)
        : Expression(NodeType::BINARY_EXPR, line, col), left(std::move(l)), operator_type(op), right(std::move(r)) {}
};

struct UnaryExpression : public Expression {
    TokenType operator_type;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(TokenType op, std::unique_ptr<Expression> expr, int l = 0, int c = 0)
        : Expression(NodeType::UNARY_EXPR, l, c), operator_type(op), operand(std::move(expr)) {}
};

struct CallExpression : public Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpression(std::unique_ptr<Expression> c, std::vector<std::unique_ptr<Expression>> args, int l = 0, int col = 0)
        : Expression(NodeType::CALL_EXPR, l, col), callee(std::move(c)), arguments(std::move(args)) {}
};

// Statement nodes
struct Statement : public ASTNode {
    Statement(NodeType t, int l = 0, int c = 0) : ASTNode(t, l, c) {}
};

struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expression;
    
    ExpressionStatement(std::unique_ptr<Expression> expr, int l = 0, int c = 0)
        : Statement(NodeType::EXPRESSION_STMT, l, c), expression(std::move(expr)) {}
};

struct AssignmentStatement : public Statement {
    std::string identifier;
    std::unique_ptr<Expression> value;
    
    AssignmentStatement(const std::string& id, std::unique_ptr<Expression> val, int l = 0, int c = 0)
        : Statement(NodeType::ASSIGNMENT_STMT, l, c), identifier(id), value(std::move(val)) {}
};

struct BlockStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
    
    BlockStatement(std::vector<std::unique_ptr<Statement>> stmts, int l = 0, int c = 0)
        : Statement(NodeType::BLOCK_STMT, l, c), statements(std::move(stmts)) {}
};

struct IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> then_branch;
    std::unique_ptr<Statement> else_branch;
    
    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStatement> then_stmt, 
                std::unique_ptr<Statement> else_stmt = nullptr, int l = 0, int c = 0)
        : Statement(NodeType::IF_STMT, l, c), condition(std::move(cond)), 
          then_branch(std::move(then_stmt)), else_branch(std::move(else_stmt)) {}
};

struct WhileStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;
    
    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStatement> b, int l = 0, int c = 0)
        : Statement(NodeType::WHILE_STMT, l, c), condition(std::move(cond)), body(std::move(b)) {}
};

struct FunctionDefStatement : public Statement {
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockStatement> body;
    
    FunctionDefStatement(const std::string& n, std::vector<std::string> params, 
                        std::unique_ptr<BlockStatement> b, int l = 0, int c = 0)
        : Statement(NodeType::FUNCTION_DEF_STMT, l, c), name(n), parameters(std::move(params)), body(std::move(b)) {}
};

struct ReturnStatement : public Statement {
    std::unique_ptr<Expression> value;
    
    ReturnStatement(std::unique_ptr<Expression> val = nullptr, int l = 0, int c = 0)
        : Statement(NodeType::RETURN_STMT, l, c), value(std::move(val)) {}
};

// Program node
struct Program : public ASTNode {
    std::vector<std::unique_ptr<Statement>> statements;
    
    Program(std::vector<std::unique_ptr<Statement>> stmts)
        : ASTNode(NodeType::PROGRAM), statements(std::move(stmts)) {}
};

// Parser class
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<Program> parse();
    
private:
    // Utility methods
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(std::vector<TokenType> types);
    void consume(TokenType type, const std::string& message);
    void synchronize();
    
    // Parsing methods
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> assignmentStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> functionDefStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<BlockStatement> blockStatement();
    
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> power();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
    
    std::vector<std::unique_ptr<Expression>> arguments();
};
