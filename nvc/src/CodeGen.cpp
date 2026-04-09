#include "CodeGen.hpp"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/TargetParser/Host.h>
#include <stdexcept>
#include <iostream>

CodeGen::CodeGen(const std::string& moduleName) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(moduleName, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

llvm::Function* CodeGen::getPrintf() {
    llvm::Function* printfFunc = module->getFunction("printf");
    if (!printfFunc) {
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            llvm::IntegerType::getInt32Ty(*context),
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(*context)),
            true
        );
        printfFunc = llvm::Function::Create(
            printfType, llvm::Function::ExternalLinkage, "printf", module.get()
        );
    }
    return printfFunc;
}

llvm::AllocaInst* CodeGen::createEntryBlockAlloca(llvm::Function* theFunction, const std::string& varName) {
    llvm::IRBuilder<> tmpBuilder(&theFunction->getEntryBlock(),
                                 theFunction->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, varName);
}

void CodeGen::generate(Program* program) {
    // Builtin declarations
    llvm::FunctionType* timeFT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), false);
    llvm::Function::Create(timeFT, llvm::Function::ExternalLinkage, "समय", module.get());

    std::vector<llvm::Type*> mathArgs(2, llvm::Type::getDoubleTy(*context));
    llvm::FunctionType* mathFT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), mathArgs, false);
    llvm::Function::Create(mathFT, llvm::Function::ExternalLinkage, "गणन", module.get());

    llvm::FunctionType* mainType = llvm::FunctionType::get(builder->getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());

    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(*context, "entry", mainFunc);
    builder->SetInsertPoint(entryBlock);

    for (const auto& stmt : program->body) {
        if (stmt->getType() == ASTNodeType::FunctionDeclaration) {
            llvm::BasicBlock* savedBlock = builder->GetInsertBlock();
            generateNode(stmt.get());
            if (savedBlock) {
                builder->SetInsertPoint(savedBlock);
            }
            continue;
        }
        generateNode(stmt.get());
    }

    builder->CreateRet(builder->getInt32(0));
}

llvm::Value* CodeGen::generateNode(ASTNode* node) {
    if (!node) return nullptr;

    switch (node->getType()) {
        case ASTNodeType::BlockStatement:
            return generateBlock(static_cast<BlockStatement*>(node));
        case ASTNodeType::VariableDeclaration:
            return generateVariableDeclaration(static_cast<VariableDeclaration*>(node));
        case ASTNodeType::ConstantDeclaration:
            return generateConstantDeclaration(static_cast<ConstantDeclaration*>(node));
        case ASTNodeType::PrintStatement:
            return generatePrintStatement(static_cast<PrintStatement*>(node));
        case ASTNodeType::IfStatement:
            return generateIfStatement(static_cast<IfStatement*>(node));
        case ASTNodeType::LoopStatement:
            return generateLoopStatement(static_cast<LoopStatement*>(node));
        case ASTNodeType::ReturnStatement:
            return generateReturnStatement(static_cast<ReturnStatement*>(node));
        case ASTNodeType::ExpressionStatement:
            return generateExpressionStatement(static_cast<ExpressionStatement*>(node));
        case ASTNodeType::BinaryExpression:
            return generateBinaryExpression(static_cast<BinaryExpression*>(node));
        case ASTNodeType::Assignment:
            return generateAssignment(static_cast<Assignment*>(node));
        case ASTNodeType::Literal:
            return generateLiteral(static_cast<Literal*>(node));
        case ASTNodeType::Identifier:
            return generateIdentifier(static_cast<Identifier*>(node));
        case ASTNodeType::CallExpression:
            return generateCallExpression(static_cast<CallExpression*>(node));
        case ASTNodeType::FunctionDeclaration:
            return generateFunctionDeclaration(static_cast<FunctionDeclaration*>(node));
        case ASTNodeType::ObjectLiteral:
            return generateObjectLiteral(static_cast<ObjectLiteral*>(node));
        case ASTNodeType::MemberAccess:
            return generateMemberAccess(static_cast<MemberAccess*>(node));
        default:
            throw std::runtime_error("Unknown AST Node Type");
    }
}

llvm::Value* CodeGen::generateBlock(BlockStatement* node) {
    llvm::Value* lastVal = nullptr;
    for (const auto& stmt : node->body) {
        lastVal = generateNode(stmt.get());
    }
    return lastVal;
}

llvm::Value* CodeGen::generateVariableDeclaration(VariableDeclaration* node) {
    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
    llvm::Type* allocType = llvm::Type::getDoubleTy(*context);
    
    // Vachana-based Memory Sizing (SUL v12.0)
    if (node->vachana == 2) {
        allocType = llvm::ArrayType::get(llvm::Type::getDoubleTy(*context), 2);
    } else if (node->vachana == 3) {
        allocType = llvm::PointerType::getUnqual(llvm::Type::getDoubleTy(*context)); // Vector pointer
    }

    llvm::AllocaInst* alloca = createEntryBlockAlloca(theFunction, node->id);
    // Explicitly re-create alloca if type is different
    if (allocType != llvm::Type::getDoubleTy(*context)) {
        llvm::IRBuilder<> tmpBuilder(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
        alloca = tmpBuilder.CreateAlloca(allocType, nullptr, node->id);
    }

    if (node->init) {
        llvm::Value* initVal = generateNode(node->init.get());
        if (!initVal) return nullptr;
        
        // Handle vector/complex initialization later
        builder->CreateStore(initVal, alloca);
    }

    namedValues[node->id] = alloca;
    return alloca;
}

llvm::Value* CodeGen::generateConstantDeclaration(ConstantDeclaration* node) {
    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
    llvm::Value* initVal = generateNode(node->init.get());

    llvm::AllocaInst* alloca;
    llvm::IRBuilder<> tmpBuilder(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
    if (initVal->getType()->isPointerTy()) {
        alloca = tmpBuilder.CreateAlloca(initVal->getType(), nullptr, node->id);
    } else {
        alloca = tmpBuilder.CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, node->id);
    }

    builder->CreateStore(initVal, alloca);
    namedValues[node->id] = alloca;
    return alloca;
}

llvm::Value* CodeGen::generatePrintStatement(PrintStatement* node) {
    llvm::Value* val = generateNode(node->expression.get());
    if (!val) return nullptr;

    llvm::Function* printfFunc = getPrintf();
    std::vector<llvm::Value*> args;

    if (val->getType()->isPointerTy()) {
        llvm::Value* formatStr = builder->CreateGlobalStringPtr("%s\n");
        args.push_back(formatStr);
        args.push_back(val);
    } else if (val->getType()->isDoubleTy()) {
        llvm::Value* formatStr = builder->CreateGlobalStringPtr("%f\n");
        args.push_back(formatStr);
        args.push_back(val);
    } else if (val->getType()->isIntegerTy(1)) {
        llvm::Value* formatStr = builder->CreateGlobalStringPtr("%d\n");
        args.push_back(formatStr);
        args.push_back(builder->CreateZExt(val, builder->getInt32Ty()));
    } else {
        throw std::runtime_error("Unsupported type for print");
    }

    return builder->CreateCall(printfFunc, args, "printfCall");
}

llvm::Value* CodeGen::generateIfStatement(IfStatement* node) {
    llvm::Value* condV = generateNode(node->condition.get());
    if (!condV) return nullptr;

    if (condV->getType()->isDoubleTy()) {
        condV = builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "ifcond");
    }

    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", theFunction);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont");

    builder->CreateCondBr(condV, thenBB, node->alternate ? elseBB : mergeBB);

    builder->SetInsertPoint(thenBB);
    generateNode(node->consequence.get());
    builder->CreateBr(mergeBB);

    theFunction->insert(theFunction->end(), elseBB);
    builder->SetInsertPoint(elseBB);
    if (node->alternate) {
        generateNode(node->alternate.get());
    }
    builder->CreateBr(mergeBB);

    theFunction->insert(theFunction->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);

    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*context));
}

llvm::Value* CodeGen::generateLoopStatement(LoopStatement* node) {
    if (node->init) {
        generateNode(node->init.get());
    }

    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopCondBB = llvm::BasicBlock::Create(*context, "loopcond", theFunction);
    llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(*context, "loopbody");
    llvm::BasicBlock* loopEndBB = llvm::BasicBlock::Create(*context, "loopend");

    builder->CreateBr(loopCondBB);
    builder->SetInsertPoint(loopCondBB);

    llvm::Value* condV = nullptr;
    if (node->test) {
        condV = generateNode(node->test.get());
        if (condV->getType()->isDoubleTy()) {
            condV = builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "loopcond");
        }
    } else {
        condV = builder->getTrue();
    }

    builder->CreateCondBr(condV, loopBodyBB, loopEndBB);

    theFunction->insert(theFunction->end(), loopBodyBB);
    builder->SetInsertPoint(loopBodyBB);

    generateNode(node->body.get());

    if (node->update) {
        generateNode(node->update.get());
    }

    builder->CreateBr(loopCondBB);

    theFunction->insert(theFunction->end(), loopEndBB);
    builder->SetInsertPoint(loopEndBB);

    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*context));
}

llvm::Value* CodeGen::generateReturnStatement(ReturnStatement* node) {
    llvm::Value* retVal = generateNode(node->argument.get());
    builder->CreateRet(retVal);
    return retVal;
}

llvm::Value* CodeGen::generateExpressionStatement(ExpressionStatement* node) {
    return generateNode(node->expression.get());
}

llvm::Value* CodeGen::generateBinaryExpression(BinaryExpression* node) {
    llvm::Value* L = generateNode(node->left.get());
    llvm::Value* R = generateNode(node->right.get());
    if (!L || !R) return nullptr;

    if (node->op == "+") return builder->CreateFAdd(L, R, "addtmp");
    if (node->op == "-") return builder->CreateFSub(L, R, "subtmp");
    if (node->op == "*") return builder->CreateFMul(L, R, "multmp");
    if (node->op == "/") return builder->CreateFDiv(L, R, "divtmp");
    if (node->op == "<") return builder->CreateFCmpULT(L, R, "cmptmp");
    if (node->op == ">") return builder->CreateFCmpUGT(L, R, "cmptmp");
    if (node->op == "<=") return builder->CreateFCmpULE(L, R, "cmptmp");
    if (node->op == ">=") return builder->CreateFCmpUGE(L, R, "cmptmp");
    if (node->op == "==") return builder->CreateFCmpUEQ(L, R, "cmptmp");
    if (node->op == "!=") return builder->CreateFCmpUNE(L, R, "cmptmp");

    throw std::runtime_error("Invalid binary operator");
}

llvm::Value* CodeGen::generateAssignment(Assignment* node) {
    llvm::Value* val = generateNode(node->right.get());
    if (!val) return nullptr;

    llvm::AllocaInst* variable = namedValues[node->left];
    if (!variable) {
        llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
        variable = createEntryBlockAlloca(theFunction, node->left);
        namedValues[node->left] = variable;
    }

    builder->CreateStore(val, variable);
    return val;
}

llvm::Value* CodeGen::generateLiteral(Literal* node) {
    if (node->isString) {
        return builder->CreateGlobalStringPtr(node->value);
    } else {
        return llvm::ConstantFP::get(*context, llvm::APFloat(std::stod(node->value)));
    }
}

llvm::Value* CodeGen::generateIdentifier(Identifier* node) {
    llvm::AllocaInst* A = namedValues[node->name];
    if (!A) {
        return llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
    }

    // SUL v12.0: Karaka-to-LLVM Semantic Dispatch
    // SAPTAMI (Adhikarana) -> Return Address (The Base Pointer - आधारः)
    if (node->role == "Adhikarana") {
        return A; // Return the pointer itself instead of loading value
    }

    // Default: Load the value (Karta/Karma behavior)
    return builder->CreateLoad(A->getAllocatedType(), A, node->name.c_str());
}

llvm::Value* CodeGen::generateMemberAccess(MemberAccess* node) {
    // SUL v11.0 Native Semantic Property Access
    // Logic: Look for mangled name 'object_property'
    std::string objName = static_cast<Identifier*>(node->object.get())->name;
    std::string mangledName = objName + "_" + node->property;
    
    llvm::AllocaInst* A = namedValues[mangledName];
    if (!A) {
        return llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
    }
    return builder->CreateLoad(A->getAllocatedType(), A, mangledName.c_str());
}

llvm::Value* CodeGen::generateObjectLiteral(ObjectLiteral* node) {
    // Basic implementation: generate stubs for properties
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*context));
}

llvm::Value* CodeGen::generateCallExpression(CallExpression* node) {
    // SUL v13.0 Native Tiṅ Call Logic
    std::string mangledName = node->callee->name;
    // Map call attempt to available mangled functions (e.g., yoga -> nava_yoga_lat)
    llvm::Function* calleeF = module->getFunction("nava_" + mangledName + "_lat");
    if (!calleeF) calleeF = module->getFunction("nava_" + mangledName + "_lrt");
    if (!calleeF) calleeF = module->getFunction(mangledName); // Fallback to raw

    if (!calleeF) {
        throw std::runtime_error("Unknown function referenced: " + mangledName);
    }

    std::vector<llvm::Value*> argsV;
    for (unsigned i = 0; i < node->arguments.size(); ++i) {
        argsV.push_back(generateNode(node->arguments[i].get()));
        if (!argsV.back()) return nullptr;
    }

    // Lṛṭ (Async) Dispatch
    if (node->lakara == "Lrt") {
        // Placeholder for Async/Promise spawn
        // In a real runtime, this would call libns_async_spawn(calleeF, argsV)
        return builder->CreateCall(calleeF, argsV, "asynctmp");
    }

    // Bahuvachana (Parallel) Dispatch
    if (node->vachana == 3) {
        // Simple Parallel Loop simulation (execute 3 times for now)
        for(int i=0; i<3; ++i) {
            builder->CreateCall(calleeF, argsV, "paralleltmp");
        }
        return llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
    }

    // SUL v14.0: Native Niyamah (Assertion) Handler
    if (mangledName == "नियमः" || mangledName == "niyamah") {
        llvm::Value* cond = argsV[0];
        if (cond->getType()->isDoubleTy()) {
            cond = builder->CreateFCmpONE(cond, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "assertcond");
        }
        
        llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
        llvm::BasicBlock* failBB = llvm::BasicBlock::Create(*context, "assert_fail", theFunction);
        llvm::BasicBlock* passBB = llvm::BasicBlock::Create(*context, "assert_pass", theFunction);
        
        builder->CreateCondBr(cond, passBB, failBB);
        builder->SetInsertPoint(failBB);
        
        // Native Trap / Panic
        llvm::Function* trap = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::trap);
        builder->CreateCall(trap, {});
        builder->CreateUnreachable();
        
        builder->SetInsertPoint(passBB);
        return llvm::ConstantFP::get(*context, llvm::APFloat(1.0)); // True
    }

    return builder->CreateCall(calleeF, argsV, "calltmp");
}

llvm::Value* CodeGen::generateFunctionDeclaration(FunctionDeclaration* node) {
    std::vector<llvm::Type*> doubles(node->params.size(), llvm::Type::getDoubleTy(*context));
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), doubles, false);
    
    // Mangled Name: nava_[id]_[lakara]
    std::string mangledName = "nava_" + node->id + "_" + (node->lakara.empty() ? "lat" : node->lakara);
    llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, mangledName, module.get());

    unsigned idx = 0;
    for (auto& arg : F->args()) {
        arg.setName(node->params[idx++]);
    }

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", F);
    builder->SetInsertPoint(BB);

    auto oldBindings = namedValues;

    for (auto& arg : F->args()) {
        llvm::AllocaInst* alloca = createEntryBlockAlloca(F, std::string(arg.getName()));
        builder->CreateStore(&arg, alloca);
        namedValues[std::string(arg.getName())] = alloca;
    }

    generateNode(node->body.get());

    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateRet(llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
    }

    namedValues = oldBindings;

    return F;
}

void CodeGen::writeObject(const std::string& filename) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        throw std::runtime_error(error);
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    std::optional<llvm::Reloc::Model> rm;
    auto theTargetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    module->setDataLayout(theTargetMachine->createDataLayout());

    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

    if (ec) {
        throw std::runtime_error("Could not open file: " + ec.message());
    }

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;

    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        throw std::runtime_error("Target machine can't emit a file of this type");
    }

    pass.run(*module);
    dest.flush();
}
