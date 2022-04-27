#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <vector>

#include "ast_node.h"
#include "ast_operand/number.h"
#include "ast_operator/add.h"
#include "ast_operator/base.h"

using std::cout;
using std::endl;
using std::move;
using std::remove_if;
// using std::unique_ptr;
// using std::vector;

namespace Spp::__Ast {

class AstTestHelper {
 public:
  static void set_nth_child(OperatorBase* node, uint n, UniqueNode v) {
    node->child_[n] = move(v);
  }
};

TEST(AstTest, CopyTest) {
  std::string x, y;
  auto a = UniqueNode(new Number(1));
  auto b = UniqueNode(new Number(2));
  cout << "a: " << *a << endl;
  cout << "b: " << *b << endl;
  auto a_copy = a->deep_copy();
  auto b_copy = b->deep_copy();
  cout << "a_copy: " << a_copy->to_string() << endl;
  cout << "b_copy: " << b_copy->to_string() << endl;
  auto c = AddOp(move(a), move(b));
  auto c_copy = c.deep_copy();
  cout << "c: " << c.to_string() << endl;
  cout << "c_copy: " << c_copy->to_string() << endl;
  x = c.to_string();
  y = c_copy->to_string();
  x.erase(remove_if(x.begin(), x.end(), isspace), x.end());
  y.erase(remove_if(y.begin(), y.end(), isspace), y.end());
  EXPECT_EQ(x, "1+2");
  EXPECT_EQ(y, "1+2");

  // Change check
  Node* base_p = c_copy.get();
  OperatorBase* op = dynamic_cast<OperatorBase*>(base_p);
  AstTestHelper::set_nth_child(op, 0, UniqueNode(new Number(3)));
  AstTestHelper::set_nth_child(op, 1, UniqueNode(new Number(4)));
  cout << "c: " << c.to_string() << endl;
  cout << "c_copy: " << c_copy->to_string() << endl;

  x = c.to_string();
  y = c_copy->to_string();
  x.erase(remove_if(x.begin(), x.end(), isspace), x.end());
  y.erase(remove_if(y.begin(), y.end(), isspace), y.end());
  EXPECT_EQ(x, "1+2");
  EXPECT_EQ(y, "3+4");
}

}  // namespace Spp::__Ast
