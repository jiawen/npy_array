#ifndef NPY_ARRAY_HALF_H_
#define NPY_ARRAY_HALF_H_

#include <ostream>

namespace npy_array {

using half = _Float16;

// operator<< overload for half.
inline std::ostream& operator<<(std::ostream& out, half arg) {
  return out << static_cast<float>(arg);
}

}  // namespace npy_array

#endif  // NPY_ARRAY_HALF_H_
