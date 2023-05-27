#pragma once
#ifndef MACHINA_AST_VISITOR_HEADER
#define MACHINA_AST_VISITOR_HEADER

#include "common.hpp"
#include "expression.hpp"
#include <llvm/IR/Constants.h>

namespace machina {
namespace ast {

/**
 * @brief
 *
 * See
 * https://cpp11.r-lib.org/articles/FAQ.html#what-are-the-underlying-types-of-cpp11-objects
 * for underlying types for R objects returned from cpp11
 */
class expression_visitor
{
  public:
    template<
      class NodeType,
      std::enable_if_t<
        std::is_base_of<expression_node, NodeType>::value,
        bool> = true>
    static llvm::Value* codegen(NodeType* const node);
};

template<>
inline llvm::Value* expression_visitor::codegen(ldouble_node* const node)
{
    return llvm::ConstantFP::get(
      *machina::ast::context, llvm::APFloat(node->value())
    );
}

// all R integers are signed 32-bit integers
template<>
inline llvm::Value* expression_visitor::codegen(linteger_node* const node)
{
    return llvm::ConstantInt::get(
      *machina::ast::context, llvm::APInt(32, node->value(), true)
    );
}

template<>
inline llvm::Value* expression_visitor::codegen(lstring_node* const node)
{
    return llvm::ConstantDataArray::getString(
      *machina::ast::context, llvm::StringRef(node->value()), false
    );
}

template<>
inline llvm::Value* expression_visitor::codegen(lraw_node* const node)
{
    return llvm::ConstantInt::get(
      *machina::ast::context, llvm::APInt(8, node->value(), false)
    );
}

template<>
inline llvm::Value* expression_visitor::codegen(lboolean_node* const node)
{
    return llvm::ConstantInt::get(
      *machina::ast::context,
      llvm::APInt(1, static_cast<int>(node->value()), false)
    );
}

} // namespace ast
} // namespace machina

#endif // MACHINA_AST_VISITOR_HEADER