#pragma once
#ifndef MACHINA_AST_COMMON_HEADER
#define MACHINA_AST_COMMON_HEADER

#include <unordered_map>
#include <llvm/IR/IRBuilder.h>

namespace machina {
namespace ast {

// references: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend

static std::unordered_map<std::string, llvm::Value*> symbol_table;
static std::unique_ptr<llvm::LLVMContext>            context;
static std::unique_ptr<llvm::Module>                 module;
static auto builder = std::make_unique<llvm::IRBuilder<>>(*context);

} // namespace ast
} // namespace machina

#endif // MACHINA_AST_COMMON_HEADER