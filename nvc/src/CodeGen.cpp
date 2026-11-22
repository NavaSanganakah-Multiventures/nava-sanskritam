#include "CodeGen.hpp"
#include "Diagnostics.hpp"
#include "Keywords.hpp"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/Intrinsics.h>
#include <optional>
#include <stdexcept>
#include <iostream>

// LLVM 18+ renamed getDeclaration -> getOrInsertDeclaration.
#if LLVM_VERSION_MAJOR >= 18
static llvm::Function* getNvcIntrinsic(llvm::Module* module, llvm::Intrinsic::ID id, llvm::ArrayRef<llvm::Type*> types = {}) {
    return llvm::Intrinsic::getOrInsertDeclaration(module, id, types);
}
#else
static llvm::Function* getNvcIntrinsic(llvm::Module* module, llvm::Intrinsic::ID id, llvm::ArrayRef<llvm::Type*> types = {}) {
    return llvm::Intrinsic::getDeclaration(module, id, types);
}
#endif

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
    llvm::Function::Create(timeFT, llvm::Function::ExternalLinkage, Keywords::SAMAYA, module.get());

    std::vector<llvm::Type*> mathArgs(2, llvm::Type::getDoubleTy(*context));
    llvm::FunctionType* mathFT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), mathArgs, false);
    llvm::Function::Create(mathFT, llvm::Function::ExternalLinkage, Keywords::GANANA, module.get());

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
            throw CompileError("अज्ञात-ग्रन्थि-प्रकारः (unknown AST node type): " + std::to_string(static_cast<int>(node->getType())));
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
        if (allocType != llvm::Type::getDoubleTy(*context)) {
            // Storing a single value into a द्विवचन/बहुवचन slot used to emit
            // invalid IR (type mismatch on store). Refuse clearly until the
            // collection model is designed.
            throw CompileError("असमर्थित-आरम्भः (dual/plural initialisation is not implemented yet): '" +
                               node->id + "' - declare it without an initialiser for now");
        }
        llvm::Value* initVal = generateNode(node->init.get());
        if (!initVal) return nullptr;
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
        llvm::Function* powF = getNvcIntrinsic(module.get(), llvm::Intrinsic::pow, {L->getType()});
        return builder->CreateCall(powF, {L, R}, "powtmp");
    }

    throw CompileError("अमान्य-द्विचक्रीय-प्रचालकः (unsupported binary operator): " + node->op);
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
    auto it = namedValues.find(node->name);
    if (it == namedValues.end()) {
        // Used to return a silent 0.0 here, which turned typos into wrong
        // programs instead of errors.
        throw CompileError("अपरिभाषित-चरः (undefined variable): '" + node->name + "'");
    }
    llvm::AllocaInst* A = it->second;

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
    auto* obj = dynamic_cast<Identifier*>(node->object.get());
    if (!obj) {
        throw CompileError("अमान्य-गुण-अभिगमः (property access on a non-name expression)");
    }

    std::string mangledName = obj->name + "_" + node->property;
    auto it = namedValues.find(mangledName);
    if (it == namedValues.end()) {
        throw CompileError("अपरिभाषित-गुणः (undefined property): '" + mangledName + "'");
    }
    llvm::AllocaInst* A = it->second;
    return builder->CreateLoad(A->getAllocatedType(), A, mangledName.c_str());
}

llvm::Value* CodeGen::generateObjectLiteral(ObjectLiteral* node) {
    // Object literals are not implemented: this used to return a null double,
    // so any program using one compiled "successfully" into meaningless code.
    (void)node;
    throw CompileError("असमर्थित-वस्तु-अक्षरम् (object literals are not implemented yet)");
}

llvm::Value* CodeGen::generateCallExpression(CallExpression* node) {
    // SUL v13.0 Native Tiṅ Call Logic
    const std::string mangledName = node->callee->name;

    // Arguments are evaluated once and reused by every dispatch path below.
    std::vector<llvm::Value*> argsV;
    argsV.reserve(node->arguments.size());
    for (unsigned i = 0; i < node->arguments.size(); ++i) {
        argsV.push_back(generateNode(node->arguments[i].get()));
        if (!argsV.back()) return nullptr;
    }

    auto expectArgs = [&](size_t expected) {
        if (argsV.size() != expected) {
            throw CompileError("अर्घता-असंगतिः (wrong number of arguments) for '" + mangledName +
                               "': expected " + std::to_string(expected) +
                               ", got " + std::to_string(argsV.size()));
        }
    };

    // ---- 1. User-defined functions, mangled as nava_<id>_<lakara> ----------
    llvm::Function* calleeF = module->getFunction("nava_" + mangledName + "_lat");
    if (!calleeF) calleeF = module->getFunction("nava_" + mangledName + "_lrt");
    if (calleeF) {
        expectArgs(calleeF->arg_size());

        // Lṛṭ (Async) Dispatch
        // NOTE: Lṛṭ is not connected to a scheduler yet, so this is still a
        // plain blocking call. Kept honest here instead of pretending.
        if (node->lakara == "Lrt") {
            return builder->CreateCall(calleeF, argsV, "asynctmp");
        }

        // Bahuvachana (Parallel) Dispatch
        // NOTE: sequential loop, not real parallelism. Same caveat as above.
        if (node->vachana == 3) {
            for (int i = 0; i < 3; ++i) {
                builder->CreateCall(calleeF, argsV, "paralleltmp");
            }
            return llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
        }

        return builder->CreateCall(calleeF, argsV, "calltmp");
    }

    // ---- 2. Builtins, dispatched by id (see Keywords.hpp) ------------------
    switch (builtinId(mangledName)) {
        case BuiltinId::Niyama: {
            // Native Niyamah (Assertion) Handler
            expectArgs(1);
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
            llvm::Function* trap = getNvcIntrinsic(module.get(), llvm::Intrinsic::trap);
            builder->CreateCall(trap, {});
            builder->CreateUnreachable();

            builder->SetInsertPoint(passBB);
            return llvm::ConstantFP::get(*context, llvm::APFloat(1.0)); // True
        }

        case BuiltinId::Jya: {
            expectArgs(1);
            llvm::Function* sinF = getNvcIntrinsic(module.get(), llvm::Intrinsic::sin, {argsV[0]->getType()});
            return builder->CreateCall(sinF, {argsV[0]}, "sintmp");
        }

        case BuiltinId::Kojya: {
            expectArgs(1);
            llvm::Function* cosF = getNvcIntrinsic(module.get(), llvm::Intrinsic::cos, {argsV[0]->getType()});
            return builder->CreateCall(cosF, {argsV[0]}, "costmp");
        }

        case BuiltinId::Vargamula: {
            expectArgs(1);
            llvm::Function* sqrtF = getNvcIntrinsic(module.get(), llvm::Intrinsic::sqrt, {argsV[0]->getType()});
            return builder->CreateCall(sqrtF, {argsV[0]}, "sqrttmp");
        }

        case BuiltinId::Samaya: {
            expectArgs(0);
            llvm::Function* timeF = module->getFunction(Keywords::SAMAYA);
            if (!timeF) {
                throw CompileError("आन्तरिक-त्रुटिः (builtin declared but missing): समय");
            }
            return builder->CreateCall(timeF, {}, "timetmp");
        }

        case BuiltinId::Trairashika:
        case BuiltinId::Ganana:
        case BuiltinId::Varga: {
            llvm::FunctionType* FT;
            const char* name;
            if (builtinId(mangledName) == BuiltinId::Trairashika) {
                expectArgs(3);
                FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context),
                                             {llvm::Type::getDoubleTy(*context),
                                              llvm::Type::getDoubleTy(*context),
                                              llvm::Type::getDoubleTy(*context)},
                                             false);
                name = Keywords::TRAIRASHIKAM;
            } else if (builtinId(mangledName) == BuiltinId::Ganana) {
                expectArgs(2);
                FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context),
                                             {llvm::Type::getDoubleTy(*context),
                                              llvm::Type::getDoubleTy(*context)},
                                             false);
                name = Keywords::GANANA;
            } else {
                expectArgs(1);
                FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context),
                                             {llvm::Type::getDoubleTy(*context)},
                                             false);
                name = Keywords::VARGAH;
            }
            llvm::FunctionCallee extF = module->getOrInsertFunction(name, FT);
            return builder->CreateCall(extF, argsV, "calltmp");
        }

        case BuiltinId::Unknown:
            break;
    }

    throw CompileError("अज्ञात-क्रिया-नाम (unknown function): '" + mangledName + "'");
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
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    std::string targetTripleStr = targetWasm ? "wasm32-unknown-unknown" : llvm::sys::getDefaultTargetTriple();

#if LLVM_VERSION_MAJOR >= 19
    llvm::Triple targetTriple(targetTripleStr);
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
#else
    module->setTargetTriple(targetTripleStr);

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTripleStr, error);

    if (!target) {
        throw std::runtime_error(error);
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    std::optional<llvm::Reloc::Model> rm;
    auto theTargetMachine = target->createTargetMachine(targetTripleStr, cpu, features, opt, rm);
#endif

    module->setDataLayout(theTargetMachine->createDataLayout());

    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

    if (ec) {
        throw CompileError(std::string("सञ्चिका उद्घाटयितुम् अशक्तः (cannot open output file): ") + ec.message());
    }

    llvm::legacy::PassManager pass;
#if LLVM_VERSION_MAJOR >= 18
    auto fileType = llvm::CodeGenFileType::ObjectFile;
#else
    auto fileType = llvm::CGFT_ObjectFile;
#endif

    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        throw CompileError("लक्ष्य-यन्त्रम् एतादृशं सञ्चिकां निर्गन्तुं न शक्नोति (target cannot emit object file)");
    }

    pass.run(*module);
    dest.flush();
}
