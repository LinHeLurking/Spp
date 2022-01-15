#include <iostream>
#include <map>

#include "Symbolic/Number.h"
#include "Symbolic/Expression.h"


using namespace std;
using namespace Spp::Numeric;
using namespace Spp;

int main() {
    Expr x{1};
    Expr y{2};
    Expr z{3};
    auto a = x + y + z;
    cout << to_string(a) << endl;
    return 0;
}
