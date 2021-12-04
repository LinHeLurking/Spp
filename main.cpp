#include <iostream>
#include <vector>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp::Expr;
using namespace Spp::Expr::Visitor;

int main() {
    auto a = NumericNode(Rational(1, 2));
    auto b = NumericNode(Rational(1, 3));
    auto c = NumericNode(Rational(1, 4));
    auto d = NumericNode(Rational(1, 5));
    auto res = (a + b) * c - c / d;
    cout << res.expr_to_string() << endl;
    cout << eval_visitor(res)->expr_to_string() << endl;
    return 0;
}
