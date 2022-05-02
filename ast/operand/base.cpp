#include "base.h"

#include <cassert>

namespace Spp::__Ast {

uint32_t OperandBase::priority() const {
  return std::numeric_limits<uint>::max();
}

UniqueNode OperandBase::expand_add(UniqueNode &&self) {
  assert(this == self.get());
  return std::move(self);
}

UniqueNode OperandBase::collect(UniqueNode &&self, uint64_t &hash) {
  assert(this == self.get());
  hash = hash_code();
  return std::move(self);
}

}  // namespace Spp::__Ast
