#include "CodeGen.hpp"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Intrinsics.h>
#if LLVM_VERSION_MAJOR >= 17
#include <llvm/TargetParser/Host.h>
#else
#include <llvm/Support/Host.h>
#endif
#include <optional>
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
            llvm::PointerType::getUnqual(*context),
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

    // Pad-Siddhi: Apply Sandhi Optimization Pass
    llvm::legacy::FunctionPassManager fpm(module.get());
    extern llvm::Pass* createSwarSandhiOptimizationPass();
    fpm.add(createSwarSandhiOptimizationPass());
    fpm.doInitialization();
    for (auto& F : *module) {
        fpm.run(F);
    }
    fpm.doFinalization();
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
        case ASTNodeType::DarshanamBlock:
            return generateDarshanamBlock(static_cast<DarshanamBlock*>(node));
        default:
            throw std::runtime_error("अज्ञात-ग्रन्थि-प्रकारः");
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
        allocType = llvm::PointerType::getUnqual(*context); // Vector pointer
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
    llvm::Function* printfFunc = getPrintf();
    
    for (size_t i = 0; i < node->expressions.size(); ++i) {
        llvm::Value* val = generateNode(node->expressions[i].get());
        if (!val) continue;

        std::vector<llvm::Value*> args;
        std::string fmt = (i == node->expressions.size() - 1) ? "" : " ";

        if (val->getType()->isPointerTy()) {
            llvm::Value* formatStr = builder->CreateGlobalString("%s" + fmt);
            args.push_back(formatStr);
            args.push_back(val);
        } else if (val->getType()->isDoubleTy()) {
            llvm::Value* formatStr = builder->CreateGlobalString("%g" + fmt);
            args.push_back(formatStr);
            args.push_back(val);
        } else if (val->getType()->isIntegerTy(1)) {
            llvm::Value* formatStr = builder->CreateGlobalString("%d" + fmt);
            args.push_back(formatStr);
            args.push_back(builder->CreateZExt(val, builder->getInt32Ty()));
        } else {
            continue;
        }

        builder->CreateCall(printfFunc, args, "printfCall");
    }

    // Add newline at the end
    builder->CreateCall(printfFunc, {builder->CreateGlobalString("\n")}, "printfCall");

    return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*context));
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

    elseBB->insertInto(theFunction);
    builder->SetInsertPoint(elseBB);
    if (node->alternate) {
        generateNode(node->alternate.get());
    }
    builder->CreateBr(mergeBB);

    mergeBB->insertInto(theFunction);
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

    loopBodyBB->insertInto(theFunction);
    builder->SetInsertPoint(loopBodyBB);

    generateNode(node->body.get());

    if (node->update) {
        generateNode(node->update.get());
    }

    builder->CreateBr(loopCondBB);

    loopEndBB->insertInto(theFunction);
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
    if (node->op == "^") {
#if LLVM_VERSION_MAJOR >= 20
        llvm::Function* powF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});
#else
        llvm::Function* powF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});
#endif
        return builder->CreateCall(powF, {L, R}, "powtmp");
    }

    throw std::runtime_error("अमान्य-द्विचक्रीय-प्रचालकः");
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
        return builder->CreateGlobalString(node->value);
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

    if (!calleeF && mangledName != "नियमः" && mangledName != "niyamah" && mangledName != "ज्या" && mangledName != "jya" && mangledName != "कोज्या" && mangledName != "kojya" && mangledName != "वर्गमूलम्" && mangledName != "vargamulam" && mangledName != "त्रैराशिकम्" && mangledName != "गणनम्" && mangledName != "वर्गः") {
        throw std::runtime_error("अज्ञात-क्रिया-नाम: " + mangledName);
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
#if LLVM_VERSION_MAJOR >= 20
        llvm::Function* trap = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::trap);
#else
        llvm::Function* trap = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::trap);
#endif
        builder->CreateCall(trap, {});
        builder->CreateUnreachable();
        
        builder->SetInsertPoint(passBB);
        return llvm::ConstantFP::get(*context, llvm::APFloat(1.0)); // True
    }

    // SUL v18.0: Native Math Intrinsics (Jya, Kojya, Vargamulam)
    if (mangledName == "ज्या" || mangledName == "jya") {
#if LLVM_VERSION_MAJOR >= 20
        llvm::Function* sinF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::sin, {argsV[0]->getType()});
#else
        llvm::Function* sinF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::sin, {argsV[0]->getType()});
#endif
        return builder->CreateCall(sinF, {argsV[0]}, "sintmp");
    }
    if (mangledName == "कोज्या" || mangledName == "kojya") {
#if LLVM_VERSION_MAJOR >= 20
        llvm::Function* cosF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::cos, {argsV[0]->getType()});
#else
        llvm::Function* cosF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::cos, {argsV[0]->getType()});
#endif
        return builder->CreateCall(cosF, {argsV[0]}, "costmp");
    }
    if (mangledName == "वर्गमूलम्" || mangledName == "vargamulam") {
#if LLVM_VERSION_MAJOR >= 20
        llvm::Function* sqrtF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::sqrt, {argsV[0]->getType()});
#else
        llvm::Function* sqrtF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::sqrt, {argsV[0]->getType()});
#endif
        return builder->CreateCall(sqrtF, {argsV[0]}, "sqrttmp");
    }
    if (mangledName == "त्रैराशिकम्" || mangledName == "गणनम्" || mangledName == "वर्गः") {
        llvm::FunctionType* FT;
        if (mangledName == "त्रैराशिकम्") {
            FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), {llvm::Type::getDoubleTy(*context), llvm::Type::getDoubleTy(*context), llvm::Type::getDoubleTy(*context)}, false);
        } else if (mangledName == "गणनम्") {
            FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), {llvm::Type::getDoubleTy(*context), llvm::Type::getDoubleTy(*context)}, false);
        } else {
            FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), {llvm::Type::getDoubleTy(*context)}, false);
        }
        llvm::FunctionCallee extF = module->getOrInsertFunction(mangledName, FT);
        return builder->CreateCall(extF, argsV, "calltmp");
    }

    return builder->CreateCall(calleeF, argsV, "calltmp");
}

llvm::Value* CodeGen::generateFunctionDeclaration(FunctionDeclaration* node) {
    std::vector<llvm::Type*> doubles(node->params.size(), llvm::Type::getDoubleTy(*context));
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), doubles, false);
    
    // Mangled Name: nava_[id]_[lakara]
    std::string lowerLakara = node->lakara;
    std::transform(lowerLakara.begin(), lowerLakara.end(), lowerLakara.begin(), ::tolower);
    std::string mangledName = "nava_" + node->id + "_" + (lowerLakara.empty() ? "lat" : lowerLakara);
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

llvm::Value* CodeGen::generateDarshanamBlock(DarshanamBlock* node) {
    // SUL UI Factory Call: SUL_UI_CreateWindow(id)
    llvm::Function* createWin = module->getFunction("SUL_UI_CreateWindow");
    if (!createWin) {
        std::vector<llvm::Type*> args = { llvm::PointerType::getUnqual(*context) };
        llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), args, false);
        createWin = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "SUL_UI_CreateWindow", module.get());
    }

    builder->CreateCall(createWin, { builder->CreateGlobalString(node->id) });

    for (const auto& el : node->elements) {
        generateDrishyamElement(el.get());
    }

    return llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
}

llvm::Value* CodeGen::generateDrishyamElement(DrishyamElement* node) {
    // SUL UI Call: SUL_UI_AddWidget(type, label, x, y, color)
    llvm::Function* addWidget = module->getFunction("SUL_UI_AddWidget");
    if (!addWidget) {
        std::vector<llvm::Type*> args = { 
            llvm::PointerType::getUnqual(*context), // type
            llvm::PointerType::getUnqual(*context), // label
            llvm::Type::getDoubleTy(*context),      // x
            llvm::Type::getDoubleTy(*context),      // y
            llvm::PointerType::getUnqual(*context)  // color
        };
        llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), args, false);
        addWidget = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "SUL_UI_AddWidget", module.get());
    }

    std::vector<llvm::Value*> argsV;
    argsV.push_back(builder->CreateGlobalString(node->type));
    argsV.push_back(builder->CreateGlobalString(node->label.empty() ? "" : node->label));
    
    // Position
    if (node->pos.size() >= 2) {
        argsV.push_back(generateNode(node->pos[0].get()));
        argsV.push_back(generateNode(node->pos[1].get()));
    } else {
        argsV.push_back(llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
        argsV.push_back(llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
    }
    
    argsV.push_back(builder->CreateGlobalString(node->color.empty() ? "None" : node->color));

    builder->CreateCall(addWidget, argsV);

    // SUL v17.0: Recursive rendering for children
    for (const auto& child : node->children) {
        generateDrishyamElement(child.get());
    }

    return llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
}

std::string CodeGen::exportAsJSON(Program* program) {
    std::stringstream ss;
    ss << "[\n";
    bool first = true;
    for (const auto& stmt : program->body) {
        if (stmt->getType() == ASTNodeType::DarshanamBlock) {
            auto darshanam = static_cast<DarshanamBlock*>(stmt.get());
            if (!first) ss << ",\n";
            ss << "  {\n";
            ss << "    \"id\": \"" << darshanam->id << "\",\n";
            ss << "    \"elements\": [\n";
            bool firstElem = true;
            for (const auto& elem : darshanam->elements) {
                if (!firstElem) ss << ",\n";
                serializeUI(elem.get(), ss, 6);
                firstElem = false;
            }
            ss << "    ]\n";
            ss << "  }";
            first = false;
        }
    }
    ss << "\n]";
    return ss.str();
}

void CodeGen::serializeUI(DrishyamElement* node, std::stringstream& ss, int indent) {
    std::string pad(indent, ' ');
    ss << pad << "{\n";
    ss << pad << "  \"type\": \"" << node->type << "\",\n";
    ss << pad << "  \"label\": \"" << node->label << "\",\n";
    ss << pad << "  \"color\": \"" << node->color << "\",\n";
    ss << pad << "  \"source\": \"" << node->source << "\",\n";
    
    ss << pad << "  \"children\": [\n";
    bool firstChar = true;
    for (const auto& child : node->children) {
        if (!firstChar) ss << ",\n";
        serializeUI(child.get(), ss, indent + 4);
        firstChar = false;
    }
    ss << "\n" << pad << "  ]\n";
    ss << pad << "}";
}

void CodeGen::writeObject(const std::string& filename) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string targetTripleStr = targetWasm ? "wasm32-unknown-unknown" : llvm::sys::getDefaultTargetTriple();
    llvm::Triple targetTriple(targetTripleStr);
#if LLVM_VERSION_MAJOR >= 17
    module->setTargetTriple(targetTripleStr);
#else
    module->setTargetTriple(targetTriple.getTriple());
#endif

    std::string error;
#if LLVM_VERSION_MAJOR >= 17
    auto target = llvm::TargetRegistry::lookupTarget(targetTripleStr, targetTriple, error);
#else
    auto target = llvm::TargetRegistry::lookupTarget(targetTripleStr, error);
#endif

    if (!target) {
        throw std::runtime_error(error);
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
#if LLVM_VERSION_MAJOR >= 16
    std::optional<llvm::Reloc::Model> rm;
#else
    llvm::Optional<llvm::Reloc::Model> rm;
#endif
#if LLVM_VERSION_MAJOR >= 17
    auto theTargetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);
#else
    auto theTargetMachine = target->createTargetMachine(targetTriple.getTriple(), cpu, features, opt, rm);
#endif

    module->setDataLayout(theTargetMachine->createDataLayout());

    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

    if (ec) {
        throw std::runtime_error("सञ्चिका उद्घाटयितुम् अशक्तः: " + ec.message());
    }

    llvm::legacy::PassManager pass;
#if LLVM_VERSION_MAJOR >= 18
    auto fileType = llvm::CodeGenFileType::ObjectFile;
#else
    auto fileType = llvm::CGFT_ObjectFile;
#endif

    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        throw std::runtime_error("लक्ष्य-यन्त्रम् एतादृशं सञ्चिकां निर्गन्तुं न शक्नोति");
    }

    pass.run(*module);
    dest.flush();
}
