#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void runInterpreter(const std::string& source) {
    try {
        // Lexical analysis
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Type: " << static_cast<int>(token.type) 
                      << ", Value: '" << token.value << "'" 
                      << ", Line: " << token.line << std::endl;
        }
        std::cout << std::endl;
        
        // Parsing
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        std::cout << "=== Parsing completed ===" << std::endl;
        std::cout << "Statements: " << program->statements.size() << std::endl;
        std::cout << std::endl;
        
        // Interpretation
        std::cout << "=== Execution ===" << std::endl;
        Interpreter interpreter;
        interpreter.interpret(*program);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Python-like Language Parser and Interpreter" << std::endl;
    std::cout << "============================================" << std::endl;
    
    if (argc > 1) {
        // Run file
        try {
            std::string source = readFile(argv[1]);
            runInterpreter(source);
        } catch (const std::exception& e) {
            std::cerr << "Error reading file: " << e.what() << std::endl;
            return 1;
        }
    } else {
        // Interactive mode / demo
        std::cout << "Running demo program..." << std::endl << std::endl;
        
        std::string demo_code = R"(
# Demo Python-like program
x = 10
y = 20
sum = x + y
print("Hello from the parser!")
print("Sum of", x, "and", y, "is", sum)

if sum > 25:
    print("Sum is greater than 25")
else:
    print("Sum is not greater than 25")

# Loop example
counter = 0
while counter < 3:
    print("Counter:", counter)
    counter = counter + 1

print("Done!")
)";
        
        runInterpreter(demo_code);
    }
    
    return 0;
}
