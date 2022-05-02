#include "base.h"

#include <cassert>

namespace Spp::__Ast {

void OperatorBase::simplify_sub_tree() {
  for (int i = 0; i < child_.size(); ++i) {
    child_[i] = std::move(child_[i]->simplify(std::move(child_[i])));
  }
}

void OperatorBase::expand_add_sub_tree() {
  for (int i = 0; i < child_.size(); ++i) {
    child_[i] = std::move(child_[i]->expand_add(std::move(child_[i])));
  }
}

// void OperatorBase::reorder_sub_tree() {
//   for (int i = 0; i < child_.size(); ++i) {
//     child_[i] = std::move(child_[i]->reorder(std::move(child_[i])));
//   }
// }

bool OperatorBase::all_child_num() const {
  for (const auto& child : child_) {
    if (child->tag() != NodeTag::Number) {
      return false;
    }
  }
  return true;
}

std::vector<OperatorBase::SmartNum> OperatorBase::get_child_num_unchecked()
    const {
  std::vector<SmartNum> ans;
  for (uint64_t i = 0; i < child_.size(); ++i) {
    ans.emplace_back(NumberAccessor::get_num_unchecked(child_[i]));
  }
  return ans;
}

uint64_t OperatorBase::combine_child_hash() const {
  uint64_t ans = 0;
  for (uint32_t i = 0; i < child_.size(); ++i) {
    ans = ans ^ (child_[i]->hash_code() << 1);
  }
  return ans;
}

const std::string& OperatorBase::name() const { return name_; }

uint32_t OperatorBase::priority() const { return priority_; }

NodeTag OperatorBase::tag() const { return NodeTag::Operator; }

UniqueNode OperatorBase::expand_add(UniqueNode&& self) {
  expand_add_sub_tree();
  return std::move(self);
}

std::string OperatorBase::to_string() const {
  std::stringstream ss;
  switch (pos_) {
    case PosType::prefix_op: {
      ss << name_;
      for (auto& child : child_) {
        ss << child->to_string();
      }
      break;
    }
    case PosType::prefix_func: {
      ss << name_ << '(';
      for (uint i = 0; i < child_.size(); ++i) {
        ss << child_[i]->to_string();
        if (i != child_.size() - 1) ss << ", ";
      }
      ss << ')';
      break;
    }
    case PosType::infix: {
      if (child_[0]->priority() < priority_) ss << '(';
      ss << child_[0]->to_string();
      if (child_[0]->priority() < priority_) ss << ')';
      if (child_.size() > 1) ss << ' ' << name_ << ' ';
      // Under most cases, there is only one child after 0.
      // But things are different for multi-add-op.
      for (uint32_t i = 1; i < child_.size(); ++i) {
        if (child_[i]->priority() < priority_) ss << '(';
        ss << child_[i]->to_string();
        if (child_[i]->priority() < priority_) ss << ')';
        if (i != child_.size() - 1) {
          ss << ' ' << name_ << ' ';
        }
      }
      break;
    }
  }
  return ss.str();
}

}  // namespace Spp::__Ast
