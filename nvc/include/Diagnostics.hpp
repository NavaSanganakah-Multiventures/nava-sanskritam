#pragma once

#include <stdexcept>
#include <string>

// CompileError is thrown for anything the compiler can detect and report
// itself. main() catches std::exception and prints it, so a CompileError is
// the difference between "your program is wrong, here is why" and a crash.
class CompileError : public std::runtime_error {
public:
    enum class Stage { Lexer, Parser, Grammar, CodeGen };

    explicit CompileError(const std::string& message, Stage stage = Stage::CodeGen)
        : std::runtime_error(message), stage_(stage) {}

    Stage stage() const { return stage_; }

private:
    Stage stage_;
};
