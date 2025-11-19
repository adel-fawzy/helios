#pragma once

#include <string>
#include <utility>

namespace helios::logging::sink {

/**
 * @class logging::sink::Interface
 *
 * @brief Logs a message to a sink.
 *
 * @details
 * - All public functions are thread-safe.
 * - All public functions are asynchronous.
 *
 * @note The caller is responsible for adding the new line character. The sink
 *       will not enter a new line to the message.
 */
class Interface {
public:
  /**
   * @brief Default constructor
   */
  Interface() = default;

  /**
   * @brief Default destructor
   */
  virtual ~Interface() = default;

  /**
   * @brief Delete copy semantics
   */
  Interface(const Interface &) = delete;
  Interface &operator=(const Interface &) = delete;

  /**
   * @brief Default move semantics
   */
  Interface(Interface &&) = default;
  Interface &operator=(Interface &&) = default;

  /**
   * @brief Writes a message to the sink
   *
   * @tparam Message The message line to write
   */
  template <typename Message> void write(Message &&msg) {
    writeImpl(std::forward<Message>(msg));
  }

protected:
  virtual void writeImpl(std::string msg) = 0;
};

} // namespace helios::logging::sink
