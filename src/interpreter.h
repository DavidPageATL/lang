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
struct Class;
struct ClassInstance;
struct Module;
using ListType = std::vector<std::shared_ptr<struct ValueWrapper>>;
using DictType = std::map<std::string, std::shared_ptr<struct ValueWrapper>>;

// Value wrapper for recursive types
struct ValueWrapper {
    std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<Function>, ListType, DictType, std::shared_ptr<Class>, std::shared_ptr<ClassInstance>, std::shared_ptr<Module>> value;
    
    ValueWrapper(const std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<Function>, ListType, DictType, std::shared_ptr<Class>, std::shared_ptr<ClassInstance>, std::shared_ptr<Module>>& v) 
        : value(v) {}
};

using Value = std::shared_ptr<ValueWrapper>;

// Return exception for function returns
class ReturnException : public std::runtime_error {
public:
    Value value;
    ReturnException(Value v) : std::runtime_error("return"), value(v) {}
};

// Runtime exception for user-defined exceptions
class RuntimeException : public std::runtime_error {
public:
    std::string exception_type;
    Value exception_value;
    
    RuntimeException(const std::string& type, Value value = nullptr, const std::string& message = "")
        : std::runtime_error(message.empty() ? type : message), 
          exception_type(type), exception_value(value) {}
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
Value makeValue(std::shared_ptr<Class> c);
Value makeValue(std::shared_ptr<ClassInstance> ci);
Value makeValue(std::shared_ptr<Module> m);

// Helper functions for value access
bool isNumber(const Value& v);
bool isString(const Value& v);
bool isBool(const Value& v);
bool isNone(const Value& v);
bool isFunction(const Value& v);
bool isList(const Value& v);
bool isDict(const Value& v);
bool isClass(const Value& v);
bool isClassInstance(const Value& v);
bool isModule(const Value& v);

double getNumber(const Value& v);
std::string getString(const Value& v);
bool getBool(const Value& v);
std::shared_ptr<Function> getFunction(const Value& v);
ListType& getList(const Value& v);
DictType& getDict(const Value& v);
std::shared_ptr<Class> getClass(const Value& v);
std::shared_ptr<ClassInstance> getClassInstance(const Value& v);
std::shared_ptr<Module> getModule(const Value& v);

// Convert value to string for printing
std::string valueToString(const Value& v);

// Helper functions for built-ins
std::string getTypeName(const Value& v);
int compareValues(const Value& a, const Value& b);

// Value type for the interpreter
struct Function {
    std::vector<std::string> parameters;
    const BlockStatement* body; // Store pointer to the original body
    std::shared_ptr<Environment> closure;
    
    Function(std::vector<std::string> params, const BlockStatement* b, std::shared_ptr<Environment> env)
        : parameters(std::move(params)), body(b), closure(env) {}
};

struct Class {
    std::string name;
    const BlockStatement* body;
    std::shared_ptr<Environment> closure;
    std::unordered_map<std::string, Value> methods;
    
    Class(const std::string& n, const BlockStatement* b, std::shared_ptr<Environment> env)
        : name(n), body(b), closure(env) {}
};

struct ClassInstance {
    std::shared_ptr<Class> classRef;
    std::unordered_map<std::string, Value> attributes;
    
    ClassInstance(std::shared_ptr<Class> c) : classRef(c) {}
};

struct Module {
    std::string name;
    std::string file_path;
    std::shared_ptr<Environment> module_env;
    std::unique_ptr<Program> ast; // Store the parsed AST to keep it alive
    
    Module() = default;
    Module(const std::string& n, const std::string& path, std::shared_ptr<Environment> env)
        : name(n), file_path(path), module_env(env) {}
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
    const std::unordered_map<std::string, Value>& getVariables() const;
};

// Interpreter class
class Interpreter {
private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    std::unordered_map<std::string, std::shared_ptr<Module>> module_cache;
    
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
    Value evaluateAttributeExpr(const AttributeExpression& expr);
    
    // Statement execution methods
    void executeClassDef(const ClassDefStatement& stmt);
    void executeImport(const ImportStatement& stmt);
    void executeFromImport(const FromImportStatement& stmt);
    void executeTry(const TryStatement& stmt);
    
    // Helper methods
    bool isTruthy(const Value& value);
    bool isEqual(const Value& a, const Value& b);
    Value performBinaryOp(TokenType op, const Value& left, const Value& right);
    Value performUnaryOp(TokenType op, const Value& operand);
    std::shared_ptr<Module> loadModule(const std::string& module_name);
    
    void setupBuiltins();
};
