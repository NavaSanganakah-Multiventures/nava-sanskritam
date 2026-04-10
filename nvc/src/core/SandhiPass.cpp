#include "CodeGen.hpp"
#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <iostream>

using namespace llvm;

namespace {
  struct SwarSandhiOptimizationPass : public FunctionPass {
    static char ID;
    SwarSandhiOptimizationPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        bool changed = false;

        // This pass simulates Pad-Siddhi (Data Promotion / Pointer Transformation)
        // If a binary instruction (add/mul) involves small types, and Sandhi applies,
        // promote it to a larger 'Dirgha' type.
        // We will mock this logic by just logging it for now, as the actual
        // Sandhi is applied at the AST/Lexer level in our compiler pipeline.

        // Let's do some dead code elimination to simulate 'Lopa' (deletion of It-Sangya)
        std::vector<Instruction*> toDelete;

        for (auto &BB : F) {
            for (auto &I : BB) {
                // If it's a zero assignment or Avagraha equivalent null logic,
                // we can mark it for deletion.
                if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    if (auto *C = dyn_cast<ConstantFP>(Store->getValueOperand())) {
                        if (C->isZero() && Store->getPointerOperand()->getName().contains("temp")) {
                            toDelete.push_back(Store);
                        }
                    }
                }
            }
        }

        for (auto *I : toDelete) {
            I->eraseFromParent();
            changed = true;
        }

        return changed;
    }
  };
}

char SwarSandhiOptimizationPass::ID = 0;

static RegisterPass<SwarSandhiOptimizationPass> X("sandhi-opt", "Nava Sanskritam Sandhi Optimization Pass", false, false);

Pass* createSwarSandhiOptimizationPass() {
    return new SwarSandhiOptimizationPass();
}
