#pragma once

#include <functional>

namespace helios::core {

/**
 * @brief Type alias for an event handler function.
 */
using Event = std::function<void()>;

} // namespace helios::core
