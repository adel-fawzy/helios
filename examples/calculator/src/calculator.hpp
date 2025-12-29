#pragma once

#include <core/active_h_object.hpp>

namespace calculator {

class Calculator : public helios::core::ActiveHObject {
public:
  helios::core::FutureResult<int>::Ptr add(int first, int second) {
    return REQ(int, {
      int result = first + second;
      fut->set(result);
    });
  }
}; // class Calculator

} // namespace calculator
