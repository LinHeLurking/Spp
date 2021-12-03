#include <iostream>
#include <vector>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp::Expression;

int main() {
    auto a = NumericNode(Rational(1, 2));
    auto b = NumericNode(Rational(1, 3));
    auto c = NumericNode(Rational(1, 4));
    auto d = NumericNode(Rational(1, 5));
    auto res = CastNode<double, Rational<int>>((a + b) * c - c / d);
    cout << res.val() << endl;
    cout << ((a + b) * c - c / d).expr_to_string() << endl;
    return 0;
}
