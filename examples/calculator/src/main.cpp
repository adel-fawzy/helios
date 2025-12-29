#include "calculator.hpp"
#include "client.hpp"

using namespace calculator;

int main() {
  auto calculator = std::make_shared<Calculator>();
  calculator::Client client(calculator);
  client.start();
}
