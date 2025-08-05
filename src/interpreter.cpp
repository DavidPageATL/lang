#include "interpreter.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cmath>

Environment::Environment(std::shared_ptr<Environment> parent) : parent(parent) {}

void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;
}

void Environment::defineBuiltin(const std::string& name, BuiltinFunction func) {
    // For simplicity, store builtin function names as special string values
    variables[name] = std::string("builtin:" + name);
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
            return num_expr.value;
        }
        
        case NodeType::STRING_EXPR: {
            const auto& str_expr = static_cast<const StringExpression&>(expr);
            return str_expr.value;
        }
        
        case NodeType::BOOLEAN_EXPR: {
            const auto& bool_expr = static_cast<const BooleanExpression&>(expr);
            return bool_expr.value;
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
        
        case NodeType::CALL_EXPR: {
            const auto& call_expr = static_cast<const CallExpression&>(expr);
            Value callee = evaluate(*call_expr.callee);
            
            std::vector<Value> arguments;
            for (const auto& arg : call_expr.arguments) {
                arguments.push_back(evaluate(*arg));
            }
            
            // Handle user-defined functions
            if (std::holds_alternative<std::shared_ptr<Function>>(callee)) {
                auto function = std::get<std::shared_ptr<Function>>(callee);
                
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
                
                Value result = nullptr;
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
            if (std::holds_alternative<std::string>(callee)) {
                std::string func_name = std::get<std::string>(callee);
                if (func_name.substr(0, 8) == "builtin:") {
                    std::string builtin_name = func_name.substr(8);
                    // Try to get the builtin function from the global environment
                    try {
                        Value builtin_val = globals->get(builtin_name);
                        if (std::holds_alternative<std::string>(builtin_val)) {
                            std::string builtin_str = std::get<std::string>(builtin_val);
                            if (builtin_str == func_name) {
                                // Handle built-in print function specifically
                                if (builtin_name == "print") {
                                    for (size_t i = 0; i < arguments.size(); ++i) {
                                        if (i > 0) std::cout << " ";
                                        std::cout << valueToString(arguments[i]);
                                    }
                                    std::cout << std::endl;
                                    return nullptr;
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
            environment->define(func_stmt.name, function);
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

std::string Interpreter::valueToString(const Value& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>) {
            // Format numbers without unnecessary decimal places
            double val = v;
            if (val == std::floor(val)) {
                return std::to_string(static_cast<int>(val));
            } else {
                return std::to_string(val);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "True" : "False";
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return "None";
        } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
            return "<function>";
        }
        return "unknown";
    }, value);
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
        }
        return true;
    }, value);
}

bool Interpreter::isEqual(const Value& a, const Value& b) {
    return a == b;
}

Value Interpreter::performBinaryOp(TokenType op, const Value& left, const Value& right) {
    switch (op) {
        case TokenType::PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) + std::get<double>(right);
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            throw std::runtime_error("Invalid operands for +");
            
        case TokenType::MINUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) - std::get<double>(right);
            }
            throw std::runtime_error("Invalid operands for -");
            
        case TokenType::MULTIPLY:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) * std::get<double>(right);
            }
            throw std::runtime_error("Invalid operands for *");
            
        case TokenType::DIVIDE:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                double r = std::get<double>(right);
                if (r == 0) throw std::runtime_error("Division by zero");
                return std::get<double>(left) / r;
            }
            throw std::runtime_error("Invalid operands for /");
            
        case TokenType::EQUAL:
            return isEqual(left, right);
            
        case TokenType::NOT_EQUAL:
            return !isEqual(left, right);
            
        case TokenType::LESS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) < std::get<double>(right);
            }
            throw std::runtime_error("Invalid operands for <");
            
        case TokenType::LESS_EQUAL:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) <= std::get<double>(right);
            }
            throw std::runtime_error("Invalid operands for <=");
            
        case TokenType::GREATER:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) > std::get<double>(right);
            }
            throw std::runtime_error("Invalid operands for >");
            
        case TokenType::GREATER_EQUAL:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) >= std::get<double>(right);
            }
            throw std::runtime_error("Invalid operands for >=");
            
        case TokenType::AND:
            return isTruthy(left) && isTruthy(right);
            
        case TokenType::OR:
            return isTruthy(left) || isTruthy(right);
            
        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

Value Interpreter::performUnaryOp(TokenType op, const Value& operand) {
    switch (op) {
        case TokenType::MINUS:
            if (std::holds_alternative<double>(operand)) {
                return -std::get<double>(operand);
            }
            throw std::runtime_error("Invalid operand for unary -");
            
        case TokenType::NOT:
            return !isTruthy(operand);
            
        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

void Interpreter::setupBuiltins() {
    // Print function
    globals->defineBuiltin("print", [](const std::vector<Value>& args) -> Value {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << std::visit([](const auto& v) -> std::string {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, double>) {
                    double val = v;
                    if (val == std::floor(val)) {
                        return std::to_string(static_cast<int>(val));
                    } else {
                        return std::to_string(val);
                    }
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return v;
                } else if constexpr (std::is_same_v<T, bool>) {
                    return v ? "True" : "False";
                } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return "None";
                } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
                    return "<function>";
                }
                return "unknown";
            }, args[i]);
        }
        std::cout << std::endl;
        return nullptr;
    });
}
