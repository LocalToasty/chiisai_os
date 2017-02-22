#include <util/delay.h>
#include "os/os.hpp"
#include "os/input.hpp"
#include "os/output.hpp"

int main() {
  os::init();

  uint8_t i = 0;

  while (true) {
    if (os::input::read() & os::input::next) {
      i = (i + 1) % 8;
    }
    if (os::input::read() & os::input::prev) {
      i = (i == 0) ? 7 : (i - 1);
    }

    os::output::write(1 << i);
    _delay_ms(150);
  }
}
