#include "os/os.hpp"
#include "os/output.hpp"
#include "os/input.hpp"

void os::init() {
  output::init();
  input::init();
}
