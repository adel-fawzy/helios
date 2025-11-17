#pragma once

#include <functional>

namespace helios::core {

/**
 * @brief Type alias for event.
 */
using Event = std::function<void()>;

} // namespace helios::core
