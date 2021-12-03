#include <iostream>
#include "Symbolic/Number.h"

using namespace std;
using namespace Spp::Numeric;

int main() {
    cout << "Hello, World!" << endl;
    auto a = Rational(1, 2);
    auto b = Rational(1, 3);
    auto c = 2;
    auto d = 3.0;
    cout << a + b << endl;
    cout << b + a << endl;
    cout << a + c << endl;
    cout << c + a << endl;
    cout << a + d << endl;
    cout << d + a << endl;
    cout << a * c << endl;
    cout << c * a << endl;
    cout << a * d << endl;
    cout << d * a << endl;

    cout << is_numeric_v<decltype(a)> << endl;
    return 0;
}
