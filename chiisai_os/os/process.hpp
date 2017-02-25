#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <stddef.h>
#include <stdint.h>

namespace os {
namespace process {

class Process;

using Program = void (*)();
using Pid = Process*;

constexpr Pid NullPid = nullptr;

//! The default stack size.
constexpr size_t default_stack_size = 0x40;

/*! The interval (in ms) in which the scheduler is called.
 *
 * Should be between 0.02 and 4.0.
 */
constexpr double scheduler_interval_ms = 1.;

/*! Initializes process submodule and starts init_proc.
 *
 * init_proc needs to be a function of type Program.
 * It is started with default_stack_size bytes of memory.
 * This function does not return.
 */
[[noreturn]] void init();

/*! Starts a new process.
 *
 * \param prog  Pointer to the program to start.
 * \param min_stack_size  The minimal stack size required by the program.
 *
 * Returns NullPid if there is not enough memory remaining to start a new
 * process;
 */
Pid exec(Program prog, size_t min_stack_size = default_stack_size);

//! Returns the id of the currently running process.
Pid get_pid();

uint8_t* beginning_of_process_stacks();
}
}

#endif /* PROCESS_HPP */