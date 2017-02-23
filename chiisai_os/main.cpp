#include "os/os.hpp"
#include "os/input.hpp"
#include "os/output.hpp"
#include "os/process.hpp"
#include "os/chrono.hpp"

void counter() {
  while (true) {
    for (int i = 0; i < 16; i++) {
      os::output::write(i << 4, 0xf0);
      os::chrono::delay_ms(1000);
    }
  }
}

void io() {
  while (true) {
    uint8_t input = os::input::read();
    os::output::write(input, 0x0f);
    os::chrono::delay_ms(10);
  }
}

void init_proc() {
  os::process::exec(counter);
  os::process::exec(io);

  // don't terminate
  while (true) {}
}