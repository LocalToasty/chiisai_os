#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdint.h>

namespace os {
namespace error {

enum Error {
  unreachable = 1,
  isr_stack_too_small,
  out_of_memory,
  assertion_failed,
};

void init();

/*! Stops program execution and displays an error code.
 *
 * If this function is called, the error light is turned on and the error code
 * displayed before execution is halted.
 */
[[noreturn]] void error(uint8_t error_code);
}
}

#endif /* ERROR_HPP */