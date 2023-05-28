#pragma once
#ifndef MACHINA_AST_FUNCTION_HEADER
#define MACHINA_AST_FUNCTION_HEADER

#include <memory>
#include "expression.hpp"

namespace machina {
namespace ast {

class prototype_node
{
    const std::string                                      m_name;
    const std::vector<std::pair<std::string, llvm::Type*>> m_args;
    llvm::Type* const                                      m_rtype;

  public:
    template<typename CharName, typename CharArgs>
    prototype_node(CharName&& name, CharArgs&& args, llvm::Type* const rtype)
      : m_name(std::forward<CharName>(name))
      , m_args(std::forward<CharArgs>(args))
      , m_rtype(rtype){};

    const auto& name() const noexcept { return this->m_name; }
    const auto& args() const noexcept { return this->m_args; }
    llvm::Type* rtype() const noexcept { return this->m_rtype; }
};

class function_node
{
    std::unique_ptr<prototype_node>  m_proto;
    std::unique_ptr<expression_node> m_definition;

  public:
    function_node(
      std::unique_ptr<prototype_node>  proto,
      std::unique_ptr<expression_node> def
    )
      : m_proto(std::move(proto))
      , m_definition(std::move(def)){};

    const auto& proto() const noexcept { return this->m_proto; }
    const auto& definition() const noexcept { return this->m_definition; }
};

} // namespace ast
} // namespace machina

#endif // MACHINA_AST_FUNCTION_HEADER