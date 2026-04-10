#pragma once

#include "AST.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <memory>
#include <string>

class CodeGen {
public:
    CodeGen(const std::string& moduleName);
    void generate(Program* program);
    void writeObject(const std::string& filename);
    std::string exportAsJSON(Program* program);

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    std::unordered_map<std::string, llvm::AllocaInst*> namedValues;

    llvm::Value* generateNode(ASTNode* node);

    llvm::Value* generateBlock(BlockStatement* node);
    llvm::Value* generateVariableDeclaration(VariableDeclaration* node);
    llvm::Value* generateConstantDeclaration(ConstantDeclaration* node);
    llvm::Value* generatePrintStatement(PrintStatement* node);
    llvm::Value* generateIfStatement(IfStatement* node);
    llvm::Value* generateLoopStatement(LoopStatement* node);
    llvm::Value* generateReturnStatement(ReturnStatement* node);
    llvm::Value* generateExpressionStatement(ExpressionStatement* node);

    llvm::Value* generateBinaryExpression(BinaryExpression* node);
    llvm::Value* generateAssignment(Assignment* node);
    llvm::Value* generateLiteral(Literal* node);
    llvm::Value* generateIdentifier(Identifier* node);
    llvm::Value* generateCallExpression(CallExpression* node);
    llvm::Value* generateFunctionDeclaration(FunctionDeclaration* node);
    llvm::Value* generateObjectLiteral(ObjectLiteral* node);
    llvm::Value* generateMemberAccess(MemberAccess* node);
    
    llvm::Value* generateDarshanamBlock(DarshanamBlock* node);
    llvm::Value* generateDrishyamElement(DrishyamElement* node);

    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* theFunction, const std::string& varName);
    llvm::Function* getPrintf();

    void serializeUI(DrishyamElement* node, std::stringstream& ss, int indent);
};
