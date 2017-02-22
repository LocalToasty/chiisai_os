#ifndef OS_INPUT_HPP
#define OS_INPUT_HPP

#include <stdint.h>

namespace os {
namespace input {

enum Button { enter = 1 << 3, next = 1 << 2, prev = 1 << 1, escape = 1 << 0 };

//! Initializes input functionality.
void init();

/*! Reads the state of one or multiple buttons.
 *
 * Input has to have been initialized prior to using this function.
 *
 * \returns the current state of the buttons, encoded as a bitmap over Button.
 */
uint8_t read();
}
}

#endif /* OS_INPUT_HPP */
