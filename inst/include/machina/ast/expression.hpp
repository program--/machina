#pragma once
#ifndef MACHINA_AST_EXPRESSION_HEADER
#define MACHINA_AST_EXPRESSION_HEADER
#include <string>
#include <cpp11/r_string.hpp>
#include <cpp11/r_bool.hpp>
#include <llvm/IR/IRBuilder.h>

namespace machina {
namespace ast {

class expression_visitor;

class expression_node
{
  public:
    virtual ~expression_node() = default;
};

template<typename T>
class literal_node : public expression_node
{
    T m_literal;

  public:
    literal_node(T value)
      : m_literal(value){};

    T value() const noexcept { return this->m_literal; }
};

using linteger_node = literal_node<int>;
using ldouble_node  = literal_node<double>;
using lstring_node  = literal_node<cpp11::r_string>;
using lboolean_node = literal_node<cpp11::r_bool>;
using lraw_node     = literal_node<uint8_t>;

class variable_node : public expression_node
{
    const std::string m_identifier;

  public:
    template<typename T>
    variable_node(T&& name)
      : m_identifier(std::forward<T>(name)){};

    const std::string& identifier() const noexcept
    {
        return this->m_identifier;
    }
};

class binary_op_node : public expression_node
{
    const char                             m_op;
    const std::unique_ptr<expression_node> m_lhs, m_rhs;

  public:
    binary_op_node(
      char                             op,
      std::unique_ptr<expression_node> lhs,
      std::unique_ptr<expression_node> rhs
    )
      : m_op(op)
      , m_lhs(std::move(lhs))
      , m_rhs(std::move(rhs)){};

    const std::unique_ptr<expression_node>& lhs() const noexcept
    {
        return this->m_lhs;
    }

    const std::unique_ptr<expression_node>& rhs() const noexcept
    {
        return this->m_rhs;
    }

    char op() const noexcept { return this->m_op; }
};

class call_node : public expression_node
{
    const std::string                                   m_call;
    const std::vector<std::unique_ptr<expression_node>> m_args;

  public:
    template<typename CharCall, typename Args>
    call_node(CharCall&& call, Args&& args)
      : m_call(std::forward<CharCall>(call))
      , m_args(std::forward<Args>(args)){};

    const std::string& caller() const noexcept { return this->m_call; }
    const auto&        args() const noexcept { return this->m_args; }
};

} // namespace ast
} // namespace machina

#endif // MACHINA_AST_EXPRESSION_HEADER