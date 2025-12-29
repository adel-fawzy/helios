#pragma once

#include <core/active_h_object.hpp>
#include <iostream>

#include "calculator.hpp"

namespace calculator {

class Client : public helios::core::ActiveHObject {
public:
  Client(std::shared_ptr<Calculator> calculator) : calculator_(calculator) {}
  void start() {
    calculator_->add(4, 7)
        THEN_POST({ std::cout << "Client received " << *result << std::endl; });
  }

private:
  std::shared_ptr<Calculator> calculator_;
}; // class Client

} // namespace calculator
