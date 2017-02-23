#include "os/chrono.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile os::chrono::Timespan ms_since_init;

void os::chrono::init() {
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS10);    // no prescaling
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  OCR1A = F_CPU / 1000;     // call timer interrupt every ms

  // reset timer
  ms_since_init = 0;
  TCNT1 = 0;
  sei();
}

//! Increases millisecond counter.
ISR(TIMER1_COMPA_vect) {
  ms_since_init++;
}

os::chrono::Timespan os::chrono::time_since_init() {
  return ms_since_init;
}

void os::chrono::delay_ms(Timespan amount) {
  Timespan target = time_since_init() + amount;
  while (time_since_init() <= target) {
  }
}
