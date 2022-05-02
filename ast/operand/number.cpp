#include "number.h"

#include "../../util/concept.h"
#include "smart_num/smart_num.h"
namespace Spp::__Ast {

std::string Number::to_string() const {
  std::stringstream ss;
  ss << value_;
  return ss.str();
}

UniqueNode Number::simplify(UniqueNode&& self) {
  assert(self.get() == this);
  value_.cast_trivial_rational();
  return std::move(self);
}

NodeTag Number::tag() const { return NodeTag::Number; }

UniqueNode Number::deep_copy() const { return UniqueNode(new Number(value_)); }

uint64_t Number::hash_code() const { return value_.hash_code(); }

}  // namespace Spp::__Ast
