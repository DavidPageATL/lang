#include "interpreter.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <fstream>
#include <filesystem>

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

Value makeValue(std::shared_ptr<Class> c) {
    return std::make_shared<ValueWrapper>(c);
}

Value makeValue(std::shared_ptr<ClassInstance> ci) {
    return std::make_shared<ValueWrapper>(ci);
}

Value makeValue(std::shared_ptr<Module> m) {
    return std::make_shared<ValueWrapper>(m);
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

bool isClass(const Value& v) {
    return std::holds_alternative<std::shared_ptr<Class>>(v->value);
}

bool isClassInstance(const Value& v) {
    return std::holds_alternative<std::shared_ptr<ClassInstance>>(v->value);
}

bool isModule(const Value& v) {
    return std::holds_alternative<std::shared_ptr<Module>>(v->value);
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

std::shared_ptr<Class> getClass(const Value& v) {
    return std::get<std::shared_ptr<Class>>(v->value);
}

std::shared_ptr<ClassInstance> getClassInstance(const Value& v) {
    return std::get<std::shared_ptr<ClassInstance>>(v->value);
}

std::shared_ptr<Module> getModule(const Value& v) {
    return std::get<std::shared_ptr<Module>>(v->value);
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
    } else if (isClass(v)) {
        auto cls = getClass(v);
        return "<class '" + cls->name + "'>";
    } else if (isClassInstance(v)) {
        auto instance = getClassInstance(v);
        return "<" + instance->classRef->name + " object>";
    } else if (isModule(v)) {
        auto module = getModule(v);
        return "<module '" + module->name + "'>";
    }
    return "<unknown>";
}

// Get the type name of a value
std::string getTypeName(const Value& v) {
    if (isNumber(v)) {
        double num = getNumber(v);
        if (num == std::floor(num)) {
            return "int";
        }
        return "float";
    } else if (isString(v)) {
        return "str";
    } else if (isBool(v)) {
        return "bool";
    } else if (isNone(v)) {
        return "NoneType";
    } else if (isFunction(v)) {
        return "function";
    } else if (isList(v)) {
        return "list";
    } else if (isDict(v)) {
        return "dict";
    } else if (isClass(v)) {
        return "type";
    } else if (isClassInstance(v)) {
        auto instance = getClassInstance(v);
        return instance->classRef->name;
    } else if (isModule(v)) {
        return "module";
    }
    return "unknown";
}

// Load a module from file
std::shared_ptr<Module> Interpreter::loadModule(const std::string& module_name) {
    // Check if module is already in cache
    auto it = module_cache.find(module_name);
    if (it != module_cache.end()) {
        return it->second;
    }
    
    // Construct file path (look for .py file)
    std::string file_path = module_name + ".py";
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("Module '" + module_name + "' not found");
    }
    
    // Read file contents
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open module file: " + file_path);
    }
    
    std::string source;
    std::string line;
    while (std::getline(file, line)) {
        source += line + "\n";
    }
    file.close();
    
    // Create module
    auto module = std::make_shared<Module>();
    module->name = module_name;
    module->file_path = file_path;
    module->module_env = std::make_shared<Environment>(globals);
    
    // Parse and execute module
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto program = parser.parse();
        
        // Store the AST in the module to keep it alive
        module->ast = std::move(program);
        
        // Save current environment
        auto saved_env = environment;
        environment = module->module_env;
        
        // Execute module in its own environment
        for (const auto& stmt : module->ast->statements) {
            execute(*stmt);
        }
        
        // Restore previous environment
        environment = saved_env;
        
        // Cache the module
        module_cache[module_name] = module;
        
        return module;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading module '" + module_name + "': " + e.what());
    }
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

const std::unordered_map<std::string, Value>& Environment::getVariables() const {
    return variables;
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
        
        case NodeType::ATTRIBUTE_EXPR: {
            return evaluateAttributeExpr(static_cast<const AttributeExpression&>(expr));
        }
        
        case NodeType::CALL_EXPR: {
            const auto& call_expr = static_cast<const CallExpression&>(expr);
            
            std::vector<Value> arguments;
            for (const auto& arg : call_expr.arguments) {
                arguments.push_back(evaluate(*arg));
            }
            
            // Check if this is a method call (obj.method())
            Value self_object = nullptr;
            if (call_expr.callee->type == NodeType::ATTRIBUTE_EXPR) {
                const auto& attr_expr = static_cast<const AttributeExpression&>(*call_expr.callee);
                Value object = evaluate(*attr_expr.object);
                if (isClassInstance(object)) {
                    self_object = object;
                }
            }
            
            Value callee = evaluate(*call_expr.callee);
            
            // Handle user-defined functions (including methods)
            if (isFunction(callee)) {
                auto function = getFunction(callee);
                
                // For method calls, prepend self to arguments
                std::vector<Value> finalArguments = arguments;
                if (self_object != nullptr) {
                    finalArguments.insert(finalArguments.begin(), self_object);
                }
                
                // Check argument count
                if (finalArguments.size() != function->parameters.size()) {
                    throw std::runtime_error("Expected " + std::to_string(function->parameters.size()) +
                                           " arguments but got " + std::to_string(finalArguments.size()));
                }
                
                // Create new environment for function execution
                auto func_env = std::make_shared<Environment>(function->closure);
                
                // Bind parameters to arguments
                for (size_t i = 0; i < function->parameters.size(); ++i) {
                    func_env->define(function->parameters[i], finalArguments[i]);
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
                    Value builtin_val = globals->get(builtin_name);
                    if (isString(builtin_val)) {
                        std::string builtin_str = getString(builtin_val);
                        if (builtin_str == func_name) {
                            // Handle built-in print function
                            if (builtin_name == "print") {
                                for (size_t i = 0; i < arguments.size(); ++i) {
                                    if (i > 0) std::cout << " ";
                                    std::cout << valueToString(arguments[i]);
                                }
                                std::cout << std::endl;
                                return makeValue(nullptr);
                            }
                            // Handle built-in raise function
                            else if (builtin_name == "raise") {
                                if (arguments.empty()) {
                                    throw RuntimeException("Exception", makeValue(nullptr), "");
                                } else if (arguments.size() == 1) {
                                    // raise("message") - throws a generic exception with message
                                    if (isString(arguments[0])) {
                                        throw RuntimeException("Exception", arguments[0], getString(arguments[0]));
                                    } else {
                                        throw RuntimeException("Exception", arguments[0], valueToString(arguments[0]));
                                    }
                                } else if (arguments.size() == 2) {
                                    // raise("ExceptionType", "message") - throws specific exception type
                                    if (!isString(arguments[0])) {
                                        throw std::runtime_error("First argument to raise() must be exception type (string)");
                                    }
                                    std::string exc_type = getString(arguments[0]);
                                    std::string message = isString(arguments[1]) ? getString(arguments[1]) : valueToString(arguments[1]);
                                    throw RuntimeException(exc_type, arguments[1], message);
                                } else {
                                    throw std::runtime_error("raise() takes 0, 1, or 2 arguments");
                                }
                                
                                return makeValue(nullptr); // Never reached
                            }
                            // Handle built-in len function
                            else if (builtin_name == "len") {
                                if (arguments.size() != 1) {
                                    throw std::runtime_error("len() takes exactly one argument");
                                }
                                Value arg = arguments[0];
                                if (isList(arg)) {
                                    auto& list = getList(arg);
                                    return makeValue(static_cast<double>(list.size()));
                                } else if (isDict(arg)) {
                                    auto& dict = getDict(arg);
                                    return makeValue(static_cast<double>(dict.size()));
                                } else if (isString(arg)) {
                                    std::string str = getString(arg);
                                    return makeValue(static_cast<double>(str.length()));
                                } else {
                                    throw std::runtime_error("object of type '" + getTypeName(arg) + "' has no len()");
                                }
                            }
                        }
                    }
                }
            }
            
            // Handle class instantiation
            if (isClass(callee)) {
                auto cls = getClass(callee);
                auto instance = std::make_shared<ClassInstance>(cls);
                
                // Call __init__ method if it exists
                auto initIt = cls->methods.find("__init__");
                if (initIt != cls->methods.end()) {
                    auto initMethod = getFunction(initIt->second);
                    
                    // Check argument count (excluding self)
                    if (arguments.size() + 1 != initMethod->parameters.size()) {
                        throw std::runtime_error("__init__ expected " + std::to_string(initMethod->parameters.size() - 1) +
                                               " arguments but got " + std::to_string(arguments.size()));
                    }
                    
                    // Create new environment for method execution
                    auto method_env = std::make_shared<Environment>(initMethod->closure);
                    
                    // Bind 'self' parameter
                    method_env->define(initMethod->parameters[0], makeValue(instance));
                    
                    // Bind other parameters to arguments
                    for (size_t i = 0; i < arguments.size(); ++i) {
                        method_env->define(initMethod->parameters[i + 1], arguments[i]);
                    }
                    
                    // Execute method body
                    std::shared_ptr<Environment> previous = environment;
                    environment = method_env;
                    
                    try {
                        for (const auto& stmt : initMethod->body->statements) {
                            execute(*stmt);
                        }
                    } catch (const ReturnException&) {
                        // Ignore return value from __init__
                    } catch (...) {
                        environment = previous;
                        throw;
                    }
                    
                    environment = previous;
                }
                
                return makeValue(instance);
            }
            
            throw std::runtime_error("Can only call functions and classes");
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
        
        case NodeType::ATTRIBUTE_ASSIGNMENT_STMT: {
            const auto& attr_assign_stmt = static_cast<const AttributeAssignmentStatement&>(stmt);
            Value object = evaluate(*attr_assign_stmt.object);
            Value value = evaluate(*attr_assign_stmt.value);
            
            if (isClassInstance(object)) {
                auto instance = getClassInstance(object);
                instance->attributes[attr_assign_stmt.attribute] = value;
            } else {
                throw std::runtime_error("Can only assign attributes to class instances");
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
        
        case NodeType::CLASS_DEF_STMT: {
            executeClassDef(static_cast<const ClassDefStatement&>(stmt));
            break;
        }
        
        case NodeType::IMPORT_STMT: {
            executeImport(static_cast<const ImportStatement&>(stmt));
            break;
        }
        
        case NodeType::FROM_IMPORT_STMT: {
            executeFromImport(static_cast<const FromImportStatement&>(stmt));
            break;
        }
        
        case NodeType::BLOCK_STMT: {
            const auto& block_stmt = static_cast<const BlockStatement&>(stmt);
            executeBlock(block_stmt.statements, environment);
            break;
        }
        
        case NodeType::TRY_STMT: {
            executeTry(static_cast<const TryStatement&>(stmt));
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
    
    // Raise function for throwing exceptions
    globals->defineBuiltin("raise", [](const std::vector<Value>& args) -> Value {
        // This lambda is not actually used, the logic is in the function call handler
        return makeValue(nullptr);
    });
    
    // Length function
    globals->defineBuiltin("len", [](const std::vector<Value>& args) -> Value {
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

Value Interpreter::evaluateAttributeExpr(const AttributeExpression& expr) {
    Value object = evaluate(*expr.object);
    
    if (isModule(object)) {
        auto module = getModule(object);
        
        // Look up the attribute in the module's environment
        try {
            Value result = module->module_env->get(expr.attribute);
            return result;
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Module '" + module->name + "' has no attribute '" + expr.attribute + "'");
        }
    } else if (isClassInstance(object)) {
        auto instance = getClassInstance(object);
        
        // First check instance attributes
        auto it = instance->attributes.find(expr.attribute);
        if (it != instance->attributes.end()) {
            return it->second;
        }
        
        // Then check class methods
        auto methodIt = instance->classRef->methods.find(expr.attribute);
        if (methodIt != instance->classRef->methods.end()) {
            return methodIt->second;
        }
        
        throw std::runtime_error("'" + instance->classRef->name + "' object has no attribute '" + expr.attribute + "'");
    }
    
    throw std::runtime_error("Object has no attributes");
}

void Interpreter::executeClassDef(const ClassDefStatement& stmt) {
    // Create class object
    auto cls = std::make_shared<Class>(stmt.name, stmt.body.get(), environment);
    
    // Execute class body in a new environment to collect methods
    auto classEnv = std::make_shared<Environment>(environment);
    auto previous = environment;
    environment = classEnv;
    
    try {
        // Execute statements directly in the class environment
        for (const auto& statement : stmt.body->statements) {
            execute(*statement);
        }
        
        // Collect all function definitions as methods
        for (const auto& [name, value] : classEnv->getVariables()) {
            if (isFunction(value)) {
                cls->methods[name] = value;
            }
        }
    } catch (...) {
        environment = previous;
        throw;
    }
    
    environment = previous;
    
    // Define the class in the current environment
    environment->define(stmt.name, makeValue(cls));
}

void Interpreter::executeImport(const ImportStatement& stmt) {
    // Load the module
    auto module = loadModule(stmt.module_name);
    
    // Define the module in the current environment
    std::string name = stmt.alias.empty() ? stmt.module_name : stmt.alias;
    environment->define(name, makeValue(module));
}

void Interpreter::executeFromImport(const FromImportStatement& stmt) {
    // Load the module
    auto module = loadModule(stmt.module_name);
    
    // Import specific symbols from the module
    for (const auto& [import_name, alias] : stmt.imports) {
        try {
            Value value = module->module_env->get(import_name);
            std::string name = alias.empty() ? import_name : alias;
            environment->define(name, value);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Cannot import '" + import_name + "' from module '" + stmt.module_name + "'");
        }
    }
}

void Interpreter::executeTry(const TryStatement& stmt) {
    try {
        // Execute the try block
        executeBlock(stmt.try_body->statements, environment);
    } catch (const RuntimeException& e) {
        // Handle user-defined exceptions
        bool handled = false;
        
        for (const auto& except_clause : stmt.except_clauses) {
            // If no exception type specified, catch all
            if (except_clause.exception_type.empty() || 
                except_clause.exception_type == e.exception_type) {
                
                // If a variable name is specified, bind the exception to it
                if (!except_clause.variable_name.empty()) {
                    environment->define(except_clause.variable_name, e.exception_value);
                }
                
                // Execute the except block
                executeBlock(except_clause.body->statements, environment);
                handled = true;
                break;
            }
        }
        
        // If no except clause handled it, re-throw
        if (!handled) {
            throw;
        }
    } catch (const std::runtime_error& e) {
        // Handle built-in runtime errors as generic exceptions
        bool handled = false;
        
        for (const auto& except_clause : stmt.except_clauses) {
            // If no exception type specified or if it's a generic RuntimeError
            if (except_clause.exception_type.empty() || 
                except_clause.exception_type == "RuntimeError") {
                
                // If a variable name is specified, bind the exception message to it
                if (!except_clause.variable_name.empty()) {
                    environment->define(except_clause.variable_name, makeValue(std::string(e.what())));
                }
                
                // Execute the except block
                executeBlock(except_clause.body->statements, environment);
                handled = true;
                break;
            }
        }
        
        // If no except clause handled it, re-throw
        if (!handled) {
            throw;
        }
    }
}
