import sys
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Revert getOrInsertDeclaration back to versioned
    content = content.replace('llvm::Function* powF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});',
                              '#if LLVM_VERSION_MAJOR >= 18\n    llvm::Function* powF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});\n#else\n    llvm::Function* powF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::pow, {L->getType()});\n#endif')

    content = content.replace('llvm::Function* trap = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::trap);',
                              '#if LLVM_VERSION_MAJOR >= 18\n    llvm::Function* trap = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::trap);\n#else\n    llvm::Function* trap = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::trap);\n#endif')

    content = content.replace('llvm::Function* sinF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::sin, {argsV[0]->getType()});',
                              '#if LLVM_VERSION_MAJOR >= 18\n    llvm::Function* sinF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::sin, {argsV[0]->getType()});\n#else\n    llvm::Function* sinF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::sin, {argsV[0]->getType()});\n#endif')

    content = content.replace('llvm::Function* cosF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::cos, {argsV[0]->getType()});',
                              '#if LLVM_VERSION_MAJOR >= 18\n    llvm::Function* cosF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::cos, {argsV[0]->getType()});\n#else\n    llvm::Function* cosF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::cos, {argsV[0]->getType()});\n#endif')

    content = content.replace('llvm::Function* sqrtF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::sqrt, {argsV[0]->getType()});',
                              '#if LLVM_VERSION_MAJOR >= 18\n    llvm::Function* sqrtF = llvm::Intrinsic::getOrInsertDeclaration(module.get(), llvm::Intrinsic::sqrt, {argsV[0]->getType()});\n#else\n    llvm::Function* sqrtF = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::sqrt, {argsV[0]->getType()});\n#endif')

    # Fix std::optional<llvm::Reloc::Model>
    content = re.sub(
        r'std::optional<llvm::Reloc::Model>\s+rm;',
        r'#if LLVM_VERSION_MAJOR >= 16\n    std::optional<llvm::Reloc::Model> rm;\n#else\n    llvm::Optional<llvm::Reloc::Model> rm;\n#endif',
        content
    )

    # Fix llvm::CodeGenFileType::ObjectFile
    content = re.sub(
        r'auto fileType = llvm::CodeGenFileType::ObjectFile;',
        r'#if LLVM_VERSION_MAJOR >= 18\n    auto fileType = llvm::CodeGenFileType::ObjectFile;\n#else\n    auto fileType = llvm::CGFT_ObjectFile;\n#endif',
        content
    )

    with open(filepath, 'w') as f:
        f.write(content)

if __name__ == "__main__":
    process_file('nvc/src/CodeGen.cpp')
