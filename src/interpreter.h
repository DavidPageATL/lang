#pragma once
#include "parser.h"
#include <unordered_map>
#include <variant>
#include <functional>
#include <memory>

// Forward declarations
struct BlockStatement;
class Environment;

// Forward declare Value for ReturnException
struct Function;
using Value = std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<Function>>;

// Return exception for function returns
class ReturnException : public std::runtime_error {
public:
    Value value;
    ReturnException(Value v) : std::runtime_error("return"), value(v) {}
};

// Built-in function type
using BuiltinFunction = std::function<Value(const std::vector<Value>&)>;

// Value type for the interpreter
struct Function {
    std::vector<std::string> parameters;
    const BlockStatement* body; // Store pointer to the original body
    std::shared_ptr<Environment> closure;
    
    Function(std::vector<std::string> params, const BlockStatement* b, std::shared_ptr<Environment> env)
        : parameters(std::move(params)), body(b), closure(env) {}
};

// Environment for variable storage
class Environment {
private:
    std::unordered_map<std::string, Value> variables;
    std::shared_ptr<Environment> parent;
    
public:
    Environment(std::shared_ptr<Environment> parent = nullptr);
    
    void define(const std::string& name, const Value& value);
    void defineBuiltin(const std::string& name, BuiltinFunction func);
    Value get(const std::string& name);
    void assign(const std::string& name, const Value& value);
};

// Interpreter class
class Interpreter {
private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    
public:
    Interpreter();
    void interpret(const Program& program);
    
private:
    Value evaluate(const Expression& expr);
    void execute(const Statement& stmt);
    void executeBlock(const std::vector<std::unique_ptr<Statement>>& statements, 
                     std::shared_ptr<Environment> env);
    
    // Helper methods
    std::string valueToString(const Value& value);
    bool isTruthy(const Value& value);
    bool isEqual(const Value& a, const Value& b);
    Value performBinaryOp(TokenType op, const Value& left, const Value& right);
    Value performUnaryOp(TokenType op, const Value& operand);
    
    void setupBuiltins();
};
