#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parser
    Parser parser(tokens);
    std::unique_ptr<ModuleAST> ast = parser.parse();
    if (!ast) {
        std::cerr << "Parsing failed.\n";
        return 1;
    }

    // 3. Code Generation
    CodeGen codegen;
    codegen.generate(*ast);
    codegen.dump();

    return 0;
}