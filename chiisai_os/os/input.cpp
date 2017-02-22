#include "os/input.hpp"
#include "os/util.hpp"
#include <avr/io.h>

using os::input::Button;
using os::util::Port;
using os::util::set_mask;
using os::util::clear_mask;

// Port the buttons are connected to
constexpr Port* input_ddr = &DDRD;
constexpr Port* input_port = &PORTD;
constexpr Port* input_pin = &PIND;

// Masks for accessing the individual buttons
constexpr uint8_t enter_mask = _BV(PORTD5);
constexpr uint8_t next_mask = _BV(PORTD4);
constexpr uint8_t prev_mask = _BV(PORTD3);
constexpr uint8_t escape_mask = _BV(PORTD2);

// Mask over all buttons
constexpr uint8_t button_mask =
    enter_mask | next_mask | prev_mask | escape_mask;

void os::input::init() {
  clear_mask(input_ddr, button_mask);  // configure as inputs
  set_mask(input_port, button_mask);   // set pull-ups
}

uint8_t os::input::read() {
  uint8_t res = 0;

  if (!(*input_pin & enter_mask)) {
    res |= Button::enter;
  }
  if (!(*input_pin & next_mask)) {
    res |= Button::next;
  }
  if (!(*input_pin & prev_mask)) {
    res |= Button::prev;
  }
  if (!(*input_pin & escape_mask)) {
    res |= Button::escape;
  }

  return res;
}
