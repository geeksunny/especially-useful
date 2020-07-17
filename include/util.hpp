#ifndef ESPECIALLY_USEFUL_INCLUDE_UTIL_HPP_
#define ESPECIALLY_USEFUL_INCLUDE_UTIL_HPP_

#include <memory>

namespace util {

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args ) {
  return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

}

#endif //ESPECIALLY_USEFUL_INCLUDE_UTIL_HPP_
