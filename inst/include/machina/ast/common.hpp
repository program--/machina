#pragma once
#ifndef MACHINA_AST_COMMON_HEADER
#define MACHINA_AST_COMMON_HEADER

#include <llvm/IR/IRBuilder.h>

namespace machina {
namespace ast {

static std::unique_ptr<llvm::LLVMContext> context;

}
} // namespace machina

#endif // MACHINA_AST_COMMON_HEADER