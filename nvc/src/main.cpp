#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeGen.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: nvc <file.ns>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();

    std::string outputFilename;
    try {
        // 1. Lexical Analysis
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parsing
        Parser parser(tokens);
        std::unique_ptr<Program> ast = parser.parse();

        // 3. Code Generation
        CodeGen codegen("nava_module");
        codegen.generate(ast.get());

        // 4. Output Object File
        outputFilename = filename.substr(0, filename.find_last_of('.')) + ".o";
        codegen.writeObject(outputFilename);

        std::cout << "Successfully compiled to " << outputFilename << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << "\n";
        return 1;
    }

    // 5. Link Final Executable
    std::string binaryName = filename.substr(0, filename.find_last_of('.'));
    // We assume nvc is run from project root, and the static library is at nvc/build/libnvc_runtime.a
    std::string command = "clang++ -no-pie " + outputFilename + " nvc/build/libnvc_runtime.a -o " + binaryName;
    int linkResult = std::system(command.c_str());

    if (linkResult == 0) {
        std::cout << "Successfully linked executable to " << binaryName << "\n";
        // Remove intermediate .o
        std::remove(outputFilename.c_str());
    } else {
        std::cerr << "Linking failed.\n";
        return 1;
    }

    return 0;
}
