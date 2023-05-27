#pragma once
#ifndef MACHINA_AST_FUNCTION_HEADER
#define MACHINA_AST_FUNCTION_HEADER

#include <memory>
#include "expression.hpp"

namespace machina {
namespace ast {

class prototype_node
{
    const std::string              m_name;
    const std::vector<std::string> m_args;

  public:
    template<typename CharName, typename CharArgs>
    prototype_node(CharName&& name, CharArgs&& args)
      : m_name(std::forward<CharName>(name))
      , m_args(std::forward<CharArgs>(args)){};

    const std::string& name() const { return this->m_name; }
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
};

} // namespace ast
} // namespace machina

#endif // MACHINA_AST_FUNCTION_HEADER