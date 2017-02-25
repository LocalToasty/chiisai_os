#ifndef OS_OUTPUT_HPP
#define OS_OUTPUT_HPP

#include <stdint.h>

namespace os {
namespace output {

//! Initialize output functionality.
void init();

//! Writes a value to the display.
void write(uint8_t value, uint8_t mask = 0xff, bool lsb_first = false);
}
}

#endif /* OS_OUTPUT_HPP */
