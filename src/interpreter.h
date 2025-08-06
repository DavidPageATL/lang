#pragma once
#include "parser.h"
#include <unordered_map>
#include <variant>
#include <functional>
#include <memory>
#include <vector>
#include <map>

// Forward declarations
struct BlockStatement;
class Environment;

// Forward declare Value and container types
struct Function;
using ListType = std::vector<std::shared_ptr<struct ValueWrapper>>;
using DictType = std::map<std::string, std::shared_ptr<struct ValueWrapper>>;

// Value wrapper for recursive types
struct ValueWrapper {
    std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<Function>, ListType, DictType> value;
    
    ValueWrapper(const std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<Function>, ListType, DictType>& v) 
        : value(v) {}
};

using Value = std::shared_ptr<ValueWrapper>;

// Return exception for function returns
class ReturnException : public std::runtime_error {
public:
    Value value;
    ReturnException(Value v) : std::runtime_error("return"), value(v) {}
};

// Built-in function type
using BuiltinFunction = std::function<Value(const std::vector<Value>&)>;

// Convenience functions for creating values
Value makeValue(double d);
Value makeValue(const std::string& s);
Value makeValue(bool b);
Value makeValue(std::nullptr_t);
Value makeValue(std::shared_ptr<Function> f);
Value makeValue(const ListType& l);
Value makeValue(const DictType& d);

// Helper functions for value access
bool isNumber(const Value& v);
bool isString(const Value& v);
bool isBool(const Value& v);
bool isNone(const Value& v);
bool isFunction(const Value& v);
bool isList(const Value& v);
bool isDict(const Value& v);

double getNumber(const Value& v);
std::string getString(const Value& v);
bool getBool(const Value& v);
std::shared_ptr<Function> getFunction(const Value& v);
ListType& getList(const Value& v);
DictType& getDict(const Value& v);

// Convert value to string for printing
std::string valueToString(const Value& v);

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
    
    // Expression evaluation methods
    Value evaluateListExpr(const ListExpression& expr);
    Value evaluateDictExpr(const DictExpression& expr);
    Value evaluateIndexExpr(const IndexExpression& expr);
    
    // Helper methods
    bool isTruthy(const Value& value);
    bool isEqual(const Value& a, const Value& b);
    Value performBinaryOp(TokenType op, const Value& left, const Value& right);
    Value performUnaryOp(TokenType op, const Value& operand);
    
    void setupBuiltins();
};
