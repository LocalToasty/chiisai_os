#ifndef OS_OUTPUT_HPP
#define OS_OUTPUT_HPP

#include <stdint.h>

namespace os {
namespace output {

//! Initialize output functionality.
void init();

//! Writes a value to the display.
void write(uint8_t value, uint8_t mask = 0xff, bool lsb_first = false);

/*! Stops program execution and displays an error code.
 *
 * If this function is called, the error light is turned on and the error code
 * displayed before execution is halted.
 */
[[noreturn]] void error(uint8_t error_code);
}
}

#endif /* OS_OUTPUT_HPP */
