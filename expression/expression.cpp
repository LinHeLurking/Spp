#include "expression.h"

#include <string>

namespace Spp::__Expression {

std::string Expression::to_string() const { return ast_->to_string(); }

inline std::ostream& operator<<(std::ostream& os, const Expression& expr) {
  os << expr.to_string();
  return os;
}

Expression&& Expression::simplify() {
  ast_ = std::move(ast_->simplify(std::move(ast_)));
  return std::move(*this);
}

Expression&& Expression::expand_add() {
  ast_ = std::move(ast_->expand_add(std::move(ast_)));
  return std::move(*this);
}

Expression&& Expression::collect() {
  uint64_t hs;
  ast_ = std::move(ast_->collect(std::move(ast_), hs));
  return std::move(*this);
}

Expression&& Expression::reorder() {
  uint64_t sz;
  ast_ = std::move(ast_->reorder(std::move(ast_), sz));
  return std::move(*this);
}

__Ast::UniqueNode Expression::take_ast(Expression&& expr) {
  return std::move(expr.ast_);
}

__Ast::UniqueNode Expression::take_ast(Expression& expr) {
  return expr.ast_->deep_copy();
}

}  // namespace Spp::__Expression
