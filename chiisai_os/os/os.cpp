#include "os/os.hpp"
#include "os/chrono.hpp"
#include "os/error.hpp"
#include "os/input.hpp"
#include "os/memory.hpp"
#include "os/output.hpp"
#include "os/process.hpp"

void os::init() {
  output::init();
  input::init();
  error::init();
  chrono::init();
  memory::init();
  process::init();
}

int main() {
  os::init();
}