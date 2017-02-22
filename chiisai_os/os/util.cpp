#include "os/util.hpp"
#include "os/output.hpp"
#include <avr/io.h>

void os::util::set_bit(Port* port, uint8_t bit) {
  *port |= _BV(bit);
}

void os::util::clear_bit(Port* port, uint8_t bit) {
  *port &= ~_BV(bit);
}

void os::util::write_bit(Port* port, uint8_t bit, bool value) {
  if (value) {
    set_bit(port, bit);
  } else {
    clear_bit(port, bit);
  }
}

void os::util::set_mask(Port* port, uint8_t mask) {
  *port |= mask;
}

void os::util::clear_mask(Port* port, uint8_t mask) {
  *port &= ~mask;
}