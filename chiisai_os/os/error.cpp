#include "os/error.hpp"
#include "os/output.hpp"
#include "os/util.hpp"
#include <avr/io.h>

using os::util::Port;

// Location of the error led
constexpr Port* error_ddr = &DDRB;
constexpr Port* error_port = &PORTB;
constexpr uint8_t error_pin = PORTB5;

void os::error::init() {
  // initialize and clear error led
  *error_ddr |= (1 << error_pin);
  *error_port &= ~(1 << error_pin);
}

void os::error::error(uint8_t error_code) {
  *error_port |= (1 << error_pin);
  os::output::write(error_code);

  // don't exit
  while (true) {
  }
}
