#include "os/output.hpp"
#include "os/os.hpp"
#include "os/util.hpp"
#include <avr/io.h>
#include <stdlib.h>

using os::util::Port;
using os::util::set_bit;
using os::util::clear_bit;
using os::util::write_bit;
using os::util::set_mask;

// Location of the error led
constexpr Port* error_ddr = &DDRB;
constexpr Port* error_port = &PORTB;
constexpr uint8_t error_pin = PORTB5;

// Location of the shift register connected to output LEDs
constexpr Port* shift_reg_ddr = &DDRB;
constexpr Port* shift_reg_port = &PORTB;
constexpr uint8_t data_pin = PORTB2;
constexpr uint8_t clock_pin = PORTB1;
constexpr uint8_t latch_pin = PORTB0;

void os::output::init() {
  // initialize and clear error led
  set_bit(error_ddr, error_pin);
  clear_bit(error_port, error_pin);

  // initialize and clear output
  set_mask(shift_reg_ddr, _BV(data_pin) | _BV(clock_pin) | _BV(latch_pin));
  clear_bit(shift_reg_port, clock_pin);
  write(0);
}

void os::output::write(uint8_t value, bool lsb_first) {
  clear_bit(shift_reg_port, latch_pin);

  for (uint8_t i = 0; i < 8; i++) {
    write_bit(shift_reg_port, data_pin,
              lsb_first ? (value & _BV(7 - i)) : (value & _BV(i)));

    set_bit(shift_reg_port, latch_pin);
    clear_bit(shift_reg_port, latch_pin);
    set_bit(shift_reg_port, clock_pin);
    clear_bit(shift_reg_port, clock_pin);
  }

  set_bit(shift_reg_port, latch_pin);
}

void os::output::error(uint8_t error_code) {
  set_bit(error_port, error_pin);
  write(error_code);
  exit(error_code);
}
