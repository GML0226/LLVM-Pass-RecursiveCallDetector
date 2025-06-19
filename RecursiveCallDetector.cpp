#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/Analysis/CallGraph.h"
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace llvm;

namespace {

class RecursiveCallDetector : public PassInfoMixin<RecursiveCallDetector> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
    // Open output file
    std::ofstream outFile("recursive_functions.txt");
    if (!outFile.is_open()) {
      errs() << "Error: Could not open recursive_functions.txt for writing\n";
      return PreservedAnalyses::all();
    }

    // Get call graph
    CallGraph &CG = MAM.getResult<CallGraphAnalysis>(M);
    
    // Iterate through strongly connected components (SCCs) to detect recursion
    for (auto I = scc_begin(&CG); !I.isAtEnd(); ++I) {
      const std::vector<CallGraphNode *> &SCC = *I;
      // If SCC has multiple nodes or a single node calls itself, it's recursive
      bool isRecursive = (SCC.size() > 1) || (SCC.size() == 1 && hasSelfCall(SCC[0]));
      
      if (isRecursive) {
        for (CallGraphNode *Node : SCC) {
          Function *F = Node->getFunction();
          if (F && !F->isDeclaration()) {
            // Record recursive call paths
            std::vector<std::vector<StringRef>> paths;
            recordRecursivePaths(F, CG, paths);
            
            // Output to file and console
            outFile << "Function '" << F->getName().str() << "' is recursive\n";
            errs() << "Function '" << F->getName() << "' is recursive\n";
            if (!paths.empty()) {
              outFile << "Recursive call paths:\n";
              errs() << "Recursive call paths:\n";
              for (const auto &path : paths) {
                outFile << "  Path: ";
                errs() << "  Path: ";
                for (size_t i = 0; i < path.size(); ++i) {
                  outFile << path[i].str();
                  errs() << path[i];
                  if (i < path.size() - 1) {
                    outFile << " -> ";
                    errs() << " -> ";
                  }
                }
                outFile << "\n";
                errs() << "\n";
              }
            } else {
              outFile << "  No detailed paths found (direct recursion)\n";
              errs() << "  No detailed paths found (direct recursion)\n";
            }
            
            // 添加空行分隔不同函数的输出
            outFile << "\n";
            errs() << "\n";
          }
        }
      }
    }
    
    // Close the file
    outFile.close();
    
    // Preserve all analyses
    return PreservedAnalyses::all();
  }

private:
  // Check if a function directly calls itself
  bool hasSelfCall(CallGraphNode *Node) {
    Function *F = Node->getFunction();
    if (!F || F->isDeclaration()) return false;
    
    for (const auto &CallRecord : *Node) {
      CallGraphNode *CalleeNode = CallRecord.second;
      Function *Callee = CalleeNode->getFunction();
      if (Callee == F) return true;
    }
    return false;
  }

  // Record recursive call paths
  void recordRecursivePaths(Function *F, CallGraph &CG, std::vector<std::vector<StringRef>> &paths) {
    std::vector<Function *> currentPath;
    std::set<Function *> visited;
    
    std::function<void(Function *)> dfs = [&](Function *current) {
      if (visited.count(current)) {
        if (current == F && !currentPath.empty()) {
          // Found a recursive path
          std::vector<StringRef> path;
          for (Function *func : currentPath) {
            path.push_back(func->getName());
          }
          path.push_back(current->getName());
          paths.push_back(path);
        }
        return;
      }
      
      visited.insert(current);
      currentPath.push_back(current);
      
      CallGraphNode *node = CG[current];
      for (const auto &CallRecord : *node) {
        Function *callee = CallRecord.second->getFunction();
        if (callee && !callee->isDeclaration()) {
          dfs(callee);
        }
      }
      
      currentPath.pop_back();
      visited.erase(current);
    };
    
    dfs(F);
  }
};

} // namespace

// Register the Pass
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "RecursiveCallDetector", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, ModulePassManager &MPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "recursive-call-detector") {
            MPM.addPass(RecursiveCallDetector());
            return true;
          }
          return false;
        });
    }
  };
}