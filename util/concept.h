#ifndef SPP_CONCEPT_H
#define SPP_CONCEPT_H

#include <type_traits>

namespace Spp::__Concept {

template <typename T>
concept SignedInteger = std::is_signed_v<T> && std::is_integral_v<T>;

template <typename T>
concept UnsignedInteger = std::is_unsigned_v<T> && std::is_integral_v<T>;

}  // namespace Spp::__Concept

#endif  // !SPP_CONCEPT_H
