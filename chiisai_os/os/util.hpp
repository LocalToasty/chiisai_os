#ifndef OS_UTIL_HPP
#define OS_UTIL_HPP

#include <stdint.h>
#include "os/input.hpp"

namespace os {
namespace util {

using Port = volatile uint8_t;

void set_bit(Port* port, uint8_t bit);
void clear_bit(Port* port, uint8_t bit);
void write_bit(Port* port, uint8_t bit, bool value);

//! Sets multiple bits of a port.
void set_mask(Port* port, uint8_t mask);
//! Clears multiple bits of a port.
void clear_mask(Port* port, uint8_t mask);

/*! Marks an unreachable point in the code.
 *
 * Displays an error and halts should it have been reached.
 */
[[noreturn]] void unreachable();
}
}

#endif /* OS_UTIL_HPP */
