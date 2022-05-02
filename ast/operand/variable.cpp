#include "variable.h"

namespace Spp::__Ast {

std::string Variable::to_string() const { return name_; }

NodeTag Variable::tag() const { return NodeTag::Variable; }

UniqueNode Variable::simplify(UniqueNode &&self) {
  assert(self.get() == this);
  return std::move(self);
}

UniqueNode Variable::deep_copy() const {
  return UniqueNode(new Variable(name_));
}

uint64_t Variable::hash_code() const { return std::hash<std::string>{}(name_); }

}  // namespace Spp::__Ast
