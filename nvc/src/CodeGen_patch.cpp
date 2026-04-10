<<<<<<< SEARCH
        llvm::Function* powF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});
=======
#if LLVM_VERSION_MAJOR >= 18
        llvm::Function* powF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});
#else
        llvm::Function* powF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});
#endif
>>>>>>> REPLACE
