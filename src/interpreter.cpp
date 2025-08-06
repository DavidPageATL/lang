#include "interpreter.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cmath>

// Convenience functions for creating values
Value makeValue(double d) {
    return std::make_shared<ValueWrapper>(d);
}

Value makeValue(const std::string& s) {
    return std::make_shared<ValueWrapper>(s);
}

Value makeValue(bool b) {
    return std::make_shared<ValueWrapper>(b);
}

Value makeValue(std::nullptr_t) {
    return std::make_shared<ValueWrapper>(nullptr);
}

Value makeValue(std::shared_ptr<Function> f) {
    return std::make_shared<ValueWrapper>(f);
}

Value makeValue(const ListType& l) {
    return std::make_shared<ValueWrapper>(l);
}

Value makeValue(const DictType& d) {
    return std::make_shared<ValueWrapper>(d);
}

// Helper functions for value access
bool isNumber(const Value& v) {
    return std::holds_alternative<double>(v->value);
}

bool isString(const Value& v) {
    return std::holds_alternative<std::string>(v->value);
}

bool isBool(const Value& v) {
    return std::holds_alternative<bool>(v->value);
}

bool isNone(const Value& v) {
    return std::holds_alternative<std::nullptr_t>(v->value);
}

bool isFunction(const Value& v) {
    return std::holds_alternative<std::shared_ptr<Function>>(v->value);
}

bool isList(const Value& v) {
    return std::holds_alternative<ListType>(v->value);
}

bool isDict(const Value& v) {
    return std::holds_alternative<DictType>(v->value);
}

double getNumber(const Value& v) {
    return std::get<double>(v->value);
}

std::string getString(const Value& v) {
    return std::get<std::string>(v->value);
}

bool getBool(const Value& v) {
    return std::get<bool>(v->value);
}

std::shared_ptr<Function> getFunction(const Value& v) {
    return std::get<std::shared_ptr<Function>>(v->value);
}

ListType& getList(const Value& v) {
    return std::get<ListType>(v->value);
}

DictType& getDict(const Value& v) {
    return std::get<DictType>(v->value);
}

// Convert value to string for printing
std::string valueToString(const Value& v) {
    if (isNumber(v)) {
        double num = getNumber(v);
        if (num == std::floor(num)) {
            return std::to_string(static_cast<int>(num));
        }
        return std::to_string(num);
    } else if (isString(v)) {
        return getString(v);
    } else if (isBool(v)) {
        return getBool(v) ? "True" : "False";
    } else if (isNone(v)) {
        return "None";
    } else if (isFunction(v)) {
        return "<function>";
    } else if (isList(v)) {
        std::string result = "[";
        const auto& list = getList(v);
        for (size_t i = 0; i < list.size(); ++i) {
            if (i > 0) result += ", ";
            result += valueToString(list[i]);
        }
        result += "]";
        return result;
    } else if (isDict(v)) {
        std::string result = "{";
        const auto& dict = getDict(v);
        bool first = true;
        for (const auto& pair : dict) {
            if (!first) result += ", ";
            result += "'" + pair.first + "': " + valueToString(pair.second);
            first = false;
        }
        result += "}";
        return result;
    }
    return "<unknown>";
}

Environment::Environment(std::shared_ptr<Environment> parent) : parent(parent) {}

void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;
}

void Environment::defineBuiltin(const std::string& name, BuiltinFunction func) {
    // For simplicity, store builtin function names as special string values
    variables[name] = makeValue(std::string("builtin:" + name));
}

Value Environment::get(const std::string& name) {
    auto var_it = variables.find(name);
    if (var_it != variables.end()) {
        return var_it->second;
    }
    
    if (parent) {
        return parent->get(name);
    }
    
    throw std::runtime_error("Undefined variable '" + name + "'");
}

void Environment::assign(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second = value;
        return;
    }
    
    if (parent) {
        parent->assign(name, value);
        return;
    }
    
    throw std::runtime_error("Undefined variable '" + name + "'");
}

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
    setupBuiltins();
}

void Interpreter::interpret(const Program& program) {
    try {
        for (const auto& stmt : program.statements) {
            execute(*stmt);
        }
    } catch (const ReturnException& ret) {
        std::cout << "Top-level return: " << valueToString(ret.value) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

Value Interpreter::evaluate(const Expression& expr) {
    switch (expr.type) {
        case NodeType::NUMBER_EXPR: {
            const auto& num_expr = static_cast<const NumberExpression&>(expr);
            return makeValue(num_expr.value);
        }
        
        case NodeType::STRING_EXPR: {
            const auto& str_expr = static_cast<const StringExpression&>(expr);
            return makeValue(str_expr.value);
        }
        
        case NodeType::BOOLEAN_EXPR: {
            const auto& bool_expr = static_cast<const BooleanExpression&>(expr);
            return makeValue(bool_expr.value);
        }
        
        case NodeType::NONE_EXPR: {
            return makeValue(nullptr);
        }
        
        case NodeType::IDENTIFIER_EXPR: {
            const auto& id_expr = static_cast<const IdentifierExpression&>(expr);
            return environment->get(id_expr.name);
        }
        
        case NodeType::BINARY_EXPR: {
            const auto& bin_expr = static_cast<const BinaryExpression&>(expr);
            Value left = evaluate(*bin_expr.left);
            Value right = evaluate(*bin_expr.right);
            return performBinaryOp(bin_expr.operator_type, left, right);
        }
        
        case NodeType::UNARY_EXPR: {
            const auto& un_expr = static_cast<const UnaryExpression&>(expr);
            Value operand = evaluate(*un_expr.operand);
            return performUnaryOp(un_expr.operator_type, operand);
        }
        
        case NodeType::LIST_EXPR: {
            return evaluateListExpr(static_cast<const ListExpression&>(expr));
        }
        
        case NodeType::DICT_EXPR: {
            return evaluateDictExpr(static_cast<const DictExpression&>(expr));
        }
        
        case NodeType::INDEX_EXPR: {
            return evaluateIndexExpr(static_cast<const IndexExpression&>(expr));
        }
        
        case NodeType::CALL_EXPR: {
            const auto& call_expr = static_cast<const CallExpression&>(expr);
            Value callee = evaluate(*call_expr.callee);
            
            std::vector<Value> arguments;
            for (const auto& arg : call_expr.arguments) {
                arguments.push_back(evaluate(*arg));
            }
            
            // Handle user-defined functions
            if (isFunction(callee)) {
                auto function = getFunction(callee);
                
                // Check argument count
                if (arguments.size() != function->parameters.size()) {
                    throw std::runtime_error("Expected " + std::to_string(function->parameters.size()) +
                                           " arguments but got " + std::to_string(arguments.size()));
                }
                
                // Create new environment for function execution
                auto func_env = std::make_shared<Environment>(function->closure);
                
                // Bind parameters to arguments
                for (size_t i = 0; i < function->parameters.size(); ++i) {
                    func_env->define(function->parameters[i], arguments[i]);
                }
                
                // Execute function body
                std::shared_ptr<Environment> previous = environment;
                environment = func_env;
                
                Value result = makeValue(nullptr);
                try {
                    for (const auto& stmt : function->body->statements) {
                        execute(*stmt);
                    }
                } catch (const ReturnException& ret) {
                    result = ret.value;
                } catch (...) {
                    environment = previous;
                    throw;
                }
                
                environment = previous;
                return result;
            }
            
            // Handle builtin functions
            if (isString(callee)) {
                std::string func_name = getString(callee);
                if (func_name.substr(0, 8) == "builtin:") {
                    std::string builtin_name = func_name.substr(8);
                    // Try to get the builtin function from the global environment
                    try {
                        Value builtin_val = globals->get(builtin_name);
                        if (isString(builtin_val)) {
                            std::string builtin_str = getString(builtin_val);
                            if (builtin_str == func_name) {
                                // Handle built-in print function specifically
                                if (builtin_name == "print") {
                                    for (size_t i = 0; i < arguments.size(); ++i) {
                                        if (i > 0) std::cout << " ";
                                        std::cout << valueToString(arguments[i]);
                                    }
                                    std::cout << std::endl;
                                    return makeValue(nullptr);
                                }
                            }
                        }
                    } catch (...) {
                        // Builtin not found, fall through to error
                    }
                }
            }
            
            throw std::runtime_error("Can only call functions");
        }
        
        default:
            throw std::runtime_error("Unknown expression type");
    }
}

void Interpreter::execute(const Statement& stmt) {
    switch (stmt.type) {
        case NodeType::EXPRESSION_STMT: {
            const auto& expr_stmt = static_cast<const ExpressionStatement&>(stmt);
            evaluate(*expr_stmt.expression);
            break;
        }
        
        case NodeType::ASSIGNMENT_STMT: {
            const auto& assign_stmt = static_cast<const AssignmentStatement&>(stmt);
            Value value = evaluate(*assign_stmt.value);
            
            // Try to assign to existing variable first, if that fails, define new one
            try {
                environment->assign(assign_stmt.identifier, value);
            } catch (const std::runtime_error&) {
                environment->define(assign_stmt.identifier, value);
            }
            break;
        }
        
        case NodeType::IF_STMT: {
            const auto& if_stmt = static_cast<const IfStatement&>(stmt);
            Value condition = evaluate(*if_stmt.condition);
            
            if (isTruthy(condition)) {
                executeBlock(if_stmt.then_branch->statements, environment);
            } else if (if_stmt.else_branch) {
                if (if_stmt.else_branch->type == NodeType::BLOCK_STMT) {
                    const auto& else_block = static_cast<const BlockStatement&>(*if_stmt.else_branch);
                    executeBlock(else_block.statements, environment);
                } else {
                    execute(*if_stmt.else_branch);
                }
            }
            break;
        }
        
        case NodeType::WHILE_STMT: {
            const auto& while_stmt = static_cast<const WhileStatement&>(stmt);
            
            while (isTruthy(evaluate(*while_stmt.condition))) {
                executeBlock(while_stmt.body->statements, environment);
            }
            break;
        }
        
        case NodeType::FOR_STMT: {
            const auto& for_stmt = static_cast<const ForStatement&>(stmt);
            Value iterable = evaluate(*for_stmt.iterable);
            
            if (isList(iterable)) {
                // Iterate over list
                const auto& list = getList(iterable);
                for (const auto& item : list) {
                    environment->define(for_stmt.variable, item);
                    executeBlock(for_stmt.body->statements, environment);
                }
            } else if (isDict(iterable)) {
                // Iterate over dictionary keys
                const auto& dict = getDict(iterable);
                for (const auto& pair : dict) {
                    environment->define(for_stmt.variable, makeValue(pair.first));
                    executeBlock(for_stmt.body->statements, environment);
                }
            } else {
                throw std::runtime_error("Object is not iterable");
            }
            break;
        }
        
        case NodeType::RETURN_STMT: {
            const auto& return_stmt = static_cast<const ReturnStatement&>(stmt);
            Value value = nullptr;
            if (return_stmt.value) {
                value = evaluate(*return_stmt.value);
            }
            throw ReturnException(value);
        }
        
        case NodeType::FUNCTION_DEF_STMT: {
            const auto& func_stmt = static_cast<const FunctionDefStatement&>(stmt);
            
            // Create function object with closure
            auto function = std::make_shared<Function>(
                func_stmt.parameters,
                func_stmt.body.get(),
                environment
            );
            
            // Define the function in the current environment
            environment->define(func_stmt.name, makeValue(function));
            break;
        }
        
        case NodeType::BLOCK_STMT: {
            const auto& block_stmt = static_cast<const BlockStatement&>(stmt);
            executeBlock(block_stmt.statements, environment);
            break;
        }
        
        default:
            throw std::runtime_error("Unknown statement type");
    }
}

void Interpreter::executeBlock(const std::vector<std::unique_ptr<Statement>>& statements, 
                              std::shared_ptr<Environment> env) {
    std::shared_ptr<Environment> previous = environment;
    
    try {
        environment = std::make_shared<Environment>(env);
        
        for (const auto& stmt : statements) {
            execute(*stmt);
        }
    } catch (...) {
        environment = previous;
        throw;
    }
    
    environment = previous;
}

bool Interpreter::isTruthy(const Value& value) {
    return std::visit([](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return false;
        } else if constexpr (std::is_same_v<T, double>) {
            return v != 0.0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return !v.empty();
        } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
            return true; // Functions are always truthy
        } else if constexpr (std::is_same_v<T, ListType>) {
            return !v.empty(); // Empty lists are falsy
        } else if constexpr (std::is_same_v<T, DictType>) {
            return !v.empty(); // Empty dicts are falsy
        }
        return true;
    }, value->value);
}

bool Interpreter::isEqual(const Value& a, const Value& b) {
    if (a->value.index() != b->value.index()) {
        return false; // Different types are not equal
    }
    
    return std::visit([&b](const auto& a_val) -> bool {
        using T = std::decay_t<decltype(a_val)>;
        if constexpr (std::is_same_v<T, ListType> || std::is_same_v<T, DictType>) {
            // For now, container equality is reference equality
            return &a_val == &std::get<T>(b->value);
        } else {
            return a_val == std::get<T>(b->value);
        }
    }, a->value);
}

Value Interpreter::performBinaryOp(TokenType op, const Value& left, const Value& right) {
    switch (op) {
        case TokenType::PLUS:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) + getNumber(right));
            }
            if (isString(left) && isString(right)) {
                return makeValue(getString(left) + getString(right));
            }
            if (isList(left) && isList(right)) {
                // List concatenation
                auto result = getList(left);
                const auto& right_list = getList(right);
                result.insert(result.end(), right_list.begin(), right_list.end());
                return makeValue(result);
            }
            throw std::runtime_error("Invalid operands for +");
            
        case TokenType::MINUS:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) - getNumber(right));
            }
            throw std::runtime_error("Invalid operands for -");
            
        case TokenType::MULTIPLY:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) * getNumber(right));
            }
            throw std::runtime_error("Invalid operands for *");
            
        case TokenType::DIVIDE:
            if (isNumber(left) && isNumber(right)) {
                double r = getNumber(right);
                if (r == 0) throw std::runtime_error("Division by zero");
                return makeValue(getNumber(left) / r);
            }
            throw std::runtime_error("Invalid operands for /");
            
        case TokenType::EQUAL:
            return makeValue(isEqual(left, right));
            
        case TokenType::NOT_EQUAL:
            return makeValue(!isEqual(left, right));
            
        case TokenType::LESS:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) < getNumber(right));
            }
            throw std::runtime_error("Invalid operands for <");
            
        case TokenType::LESS_EQUAL:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) <= getNumber(right));
            }
            throw std::runtime_error("Invalid operands for <=");
            
        case TokenType::GREATER:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) > getNumber(right));
            }
            throw std::runtime_error("Invalid operands for >");
            
        case TokenType::GREATER_EQUAL:
            if (isNumber(left) && isNumber(right)) {
                return makeValue(getNumber(left) >= getNumber(right));
            }
            throw std::runtime_error("Invalid operands for >=");
            
        case TokenType::AND:
            return makeValue(isTruthy(left) && isTruthy(right));
            
        case TokenType::OR:
            return makeValue(isTruthy(left) || isTruthy(right));
            
        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

Value Interpreter::performUnaryOp(TokenType op, const Value& operand) {
    switch (op) {
        case TokenType::MINUS:
            if (isNumber(operand)) {
                return makeValue(-getNumber(operand));
            }
            throw std::runtime_error("Invalid operand for unary -");
            
        case TokenType::NOT:
            return makeValue(!isTruthy(operand));
            
        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

void Interpreter::setupBuiltins() {
    // Print function
    globals->defineBuiltin("print", [](const std::vector<Value>& args) -> Value {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << valueToString(args[i]);
        }
        std::cout << std::endl;
        return makeValue(nullptr);
    });
}

// New expression evaluation methods
Value Interpreter::evaluateListExpr(const ListExpression& expr) {
    ListType list;
    for (const auto& elem : expr.elements) {
        list.push_back(evaluate(*elem));
    }
    return makeValue(list);
}

Value Interpreter::evaluateDictExpr(const DictExpression& expr) {
    DictType dict;
    for (const auto& pair : expr.pairs) {
        Value key = evaluate(*pair.first);
        Value value = evaluate(*pair.second);
        
        // Keys must be strings for now
        if (!isString(key)) {
            throw std::runtime_error("Dictionary keys must be strings");
        }
        
        dict[getString(key)] = value;
    }
    return makeValue(dict);
}

Value Interpreter::evaluateIndexExpr(const IndexExpression& expr) {
    Value object = evaluate(*expr.object);
    Value index = evaluate(*expr.index);
    
    if (isList(object)) {
        if (!isNumber(index)) {
            throw std::runtime_error("List indices must be integers");
        }
        
        auto& list = getList(object);
        int idx = static_cast<int>(getNumber(index));
        
        // Handle negative indices
        if (idx < 0) {
            idx += list.size();
        }
        
        if (idx < 0 || idx >= static_cast<int>(list.size())) {
            throw std::runtime_error("List index out of range");
        }
        
        return list[idx];
    } else if (isDict(object)) {
        if (!isString(index)) {
            throw std::runtime_error("Dictionary keys must be strings");
        }
        
        auto& dict = getDict(object);
        std::string key = getString(index);
        
        auto it = dict.find(key);
        if (it == dict.end()) {
            throw std::runtime_error("Key '" + key + "' not found in dictionary");
        }
        
        return it->second;
    } else {
        throw std::runtime_error("Object is not subscriptable");
    }
}
