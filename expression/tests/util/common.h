#include <algorithm>
#include <string>

inline std::string& remove_whitespace(std::string& s) {
  s.erase(remove(s.begin(), s.end(), ' '), s.end());
  return s;
}
