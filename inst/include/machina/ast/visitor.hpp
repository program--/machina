#pragma once
#include "machina/ast/function.hpp"
#include <llvm/IR/Value.h>
#ifndef MACHINA_AST_VISITOR_HEADER
#define MACHINA_AST_VISITOR_HEADER

#include "common.hpp"
#include "expression.hpp"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>

namespace machina {
namespace ast {

/**
 * @brief
 *
 * See
 * https://cpp11.r-lib.org/articles/FAQ.html#what-are-the-underlying-types-of-cpp11-objects
 * for underlying types for R objects returned from cpp11
 */
class visitor
{
  public:
    template<
      class NodeType,
      std::enable_if_t<
        std::is_base_of<expression_node, NodeType>::value,
        bool> = true>
    static llvm::Value* codegen(NodeType* const node);

    template<
      class NodeType,
      std::enable_if_t<
        std::is_base_of<expression_node, NodeType>::value,
        bool> = true>
    static llvm::Value* codegen(const std::unique_ptr<NodeType>& node)
    {
        return visitor::codegen(node.get());
    }

    template<
      class FNodeType,
      std::enable_if_t<
        std::is_same<prototype_node, FNodeType>::value ||
          std::is_same<function_node, FNodeType>::value,
        bool> = true>
    static llvm::Function* codegen(FNodeType* const node);

    template<
      class FNodeType,
      std::enable_if_t<
        std::is_same<prototype_node, FNodeType>::value ||
          std::is_same<function_node, FNodeType>::value,
        bool> = true>
    static llvm::Function* codegen(const std::unique_ptr<FNodeType>& node)
    {
        return visitor::codegen(node.get());
    }
};

// LITERALS ===================================================================

template<>
inline llvm::Value* visitor::codegen(ldouble_node* const node)
{
    return llvm::ConstantFP::get(
      *machina::ast::context, llvm::APFloat(node->value())
    );
}

// all R integers are signed 32-bit integers
template<>
inline llvm::Value* visitor::codegen(linteger_node* const node)
{
    return llvm::ConstantInt::get(
      *machina::ast::context, llvm::APInt(32, node->value(), true)
    );
}

template<>
inline llvm::Value* visitor::codegen(lstring_node* const node)
{
    return llvm::ConstantDataArray::getString(
      *machina::ast::context, llvm::StringRef(node->value()), false
    );
}

template<>
inline llvm::Value* visitor::codegen(lraw_node* const node)
{
    return llvm::ConstantInt::get(
      *machina::ast::context, llvm::APInt(8, node->value(), false)
    );
}

template<>
inline llvm::Value* visitor::codegen(lboolean_node* const node)
{
    return llvm::ConstantInt::get(
      *machina::ast::context,
      llvm::APInt(1, static_cast<int>(node->value()), false)
    );
}

// VARIABLE ===================================================================

template<>
inline llvm::Value* visitor::codegen(variable_node* const node)
{
    llvm::Value* v = machina::ast::symbol_table.at(node->identifier());
    if (!v) {
        fprintf(
          stderr,
          "Error: unknown variable name '%s'\n",
          node->identifier().c_str()
        );
        return nullptr;
    }

    return v;
}

// BINARY OP ==================================================================

template<>
inline llvm::Value* visitor::codegen(binary_op_node* const node)
{
    llvm::Value* l = visitor::codegen(node->lhs());
    llvm::Value* r = visitor::codegen(node->rhs());
    if (!l || !r) {
        return nullptr;
    }

    return nullptr;

    // TODO:
    // Compare LHS and RHS types before continuing
    // i.e. for Floats:
    // switch (node->op()) {
    //     case '+':
    //         return builder->CreateFAdd(l, r, "add");
    //     case '-':
    //         return builder->Create
    //     case '*':
    //         return builder->Create
    //     case '<':
    //         return builder->CreateFCmpULT(l, r, "ltcmp");
    // }
}

template<>
inline llvm::Value* visitor::codegen(call_node* const node)
{
    llvm::Function* caller = module->getFunction(node->caller());
    if (!caller) {
        return nullptr;
    }

    if (caller->arg_size() != node->args().size()) {
        return nullptr;
    }

    std::vector<llvm::Value*> args_v;
    args_v.reserve(node->args().size());
    for (auto& arg : node->args()) {
        args_v.push_back(visitor::codegen(arg));
        if (!args_v.back()) {
            return nullptr;
        }
    }

    return builder->CreateCall(caller, args_v, "call_" + node->caller());
}

//

template<>
inline llvm::Function* visitor::codegen(prototype_node* const node)
{
    std::vector<llvm::Type*> types;
    types.reserve(node->args().size());
    for (auto&& p : node->args()) {
        types.push_back(p.second);
    }

    // TODO: R supports variadic arguments, need to check for this
    auto ft = llvm::FunctionType::get(node->rtype(), types, false);

    auto f  = llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, node->name(), module.get()
    );

    size_t i = 0;
    for (auto& arg : f->args()) {
        arg.setName(node->args().at(i++).first);
    }

    return f;
}

template<>
inline llvm::Function* visitor::codegen(function_node* const node)
{
    llvm::Function* func = module->getFunction(node->proto()->name());

    if (!func) {
        func = visitor::codegen(node->proto());
    }

    if (!func) {
        return nullptr;
    }

    // TODO: already defined, can't redefine (per LLVM's tutorial, but R allows
    // redefinition in all scopes)
    if (!func->empty()) {
        return nullptr;
    }

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(block);

    symbol_table.clear();
    for (auto& arg : func->args()) {
        symbol_table[arg.getName().str()] = &arg;
    }

    if (llvm::Value* ret = visitor::codegen(node->definition())) {
        builder->CreateRet(ret);

        llvm::verifyFunction(*func);

        return func;
    }

    func->eraseFromParent();
    return nullptr;
}

} // namespace ast
} // namespace machina

#endif // MACHINA_AST_VISITOR_HEADER