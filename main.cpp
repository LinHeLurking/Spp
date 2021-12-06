#include <iostream>
#include <map>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp;

int main() {
    Var x{"x"};
    Constant a = 1;
    Expr y = a / (a + x);
    cout << y << endl;
    cout << y.substitute({{"x", y}}) << endl;
    cout << y.substitute({{"x", y}}).eval({{"x", 1}}) << endl;
    return 0;
}
