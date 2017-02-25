#include "os/process.hpp"

#include "os/memory.hpp"
#include "os/output.hpp"
#include "os/util.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include <string.h>

/*! Saves the a process's context.
 *
 * \param proc  Process whose state is to be saved.
 *
 * This function must called first in a naked ISR, as it expects the first entry
 * on the stack to be the current program counter.
 */
#define SAVE_CONTEXT(proc)   \
  asm volatile(              \
      "push r0\n\t"          \
      "push r1\n\t"          \
      "push r2\n\t"          \
      "push r3\n\t"          \
      "push r4\n\t"          \
      "push r5\n\t"          \
      "push r6\n\t"          \
      "push r7\n\t"          \
      "push r8\n\t"          \
      "push r9\n\t"          \
      "push r10\n\t"         \
      "push r11\n\t"         \
      "push r12\n\t"         \
      "push r13\n\t"         \
      "push r14\n\t"         \
      "push r15\n\t"         \
      "push r16\n\t"         \
      "push r17\n\t"         \
      "push r18\n\t"         \
      "push r19\n\t"         \
      "push r20\n\t"         \
      "push r21\n\t"         \
      "push r22\n\t"         \
      "push r23\n\t"         \
      "push r24\n\t"         \
      "push r25\n\t"         \
      "push r26\n\t"         \
      "push r27\n\t"         \
      "push r28\n\t"         \
      "push r29\n\t"         \
      "push r30\n\t"         \
      "push r31\n\t"         \
      "in r31, __SREG__\n\t" \
      "push r31\n\t" ::);    \
  (proc)->stack_top = (uint8_t*)SP

/*! Restores a process's context and continues its execution.
 *
 * \param proc  Process whose context is to be restored.
 *
 * This process has to be called last in a function, as it returns from it.
 * Upon returning, interrupts are disabled.
 */
#define RESTORE_CONTEXT(proc)     \
  SP = (uint16_t)proc->stack_top; \
  asm volatile(                   \
      "pop r31\n\t"               \
      "out __SREG__, r31\n\t"     \
      "pop r31\n\t"               \
      "pop r30\n\t"               \
      "pop r29\n\t"               \
      "pop r28\n\t"               \
      "pop r27\n\t"               \
      "pop r26\n\t"               \
      "pop r25\n\t"               \
      "pop r24\n\t"               \
      "pop r23\n\t"               \
      "pop r22\n\t"               \
      "pop r21\n\t"               \
      "pop r20\n\t"               \
      "pop r19\n\t"               \
      "pop r18\n\t"               \
      "pop r17\n\t"               \
      "pop r16\n\t"               \
      "pop r15\n\t"               \
      "pop r14\n\t"               \
      "pop r13\n\t"               \
      "pop r12\n\t"               \
      "pop r11\n\t"               \
      "pop r10\n\t"               \
      "pop r9\n\t"                \
      "pop r8\n\t"                \
      "pop r7\n\t"                \
      "pop r6\n\t"                \
      "pop r5\n\t"                \
      "pop r4\n\t"                \
      "pop r3\n\t"                \
      "pop r2\n\t"                \
      "pop r1\n\t"                \
      "pop r0\n\t"                \
      "reti" ::)

using os::process::Process;

/*! A process control block.
 *
 * Processes
 */
struct Process {
  //! The state of a process.
  enum class State {
    //! Currently not used and will be ignored by the scheduler.
    unused,
    //! Ready to be run.
    ready,
  };

  /*! The pointer to the next control block.
   *
   * The address of next has to be less than that of this.
   * If next is a nullptr, this is the last element of the process control block
   * queue, and its state needs to be set to unused.
   */
  Process* next{nullptr};

  //! The state of this process.
  State state{State::unused};

  /*! The stack pointer of this process.
   *
   * This value is updated every time SAVE_CONTEXT(this) is called.
   */
  uint8_t* stack_top{nullptr};

  //! Finds the first unused process after (and including) this one.
  Process* find_first_unused(size_t min_stack_size);

  //! Returns the processes stack size usable for data.
  size_t stack_size();
};

Process* scheduler();

constexpr size_t context_size = 2 + 32 + 1;  // PC + R0-R31 + SREG

Process* root_process;
Process* current_process = nullptr;

extern void init_proc();

void os::process::init() {
  // make space for first two process stacks
  SP -= 2 * sizeof(Process) + context_size;

  // initialize root process
  root_process = (Process*)(RAMEND - sizeof(Process));
  *root_process = Process();

  // set up init process
  current_process = exec(init_proc, default_stack_size);

  // set up the timer calling the scheduler
  TCCR0B |= (1 << WGM02);   // clear timer on compare
  TCCR0B |= (1 << CS02);    // 256 prescaler
  TIMSK0 |= (1 << OCIE0B);  // enable timer compare interrupt
  constexpr uint8_t threshold = 62.5 * scheduler_interval_ms;
  OCR0B = threshold;

  // reset timer
  TCNT0 = 0;

  // run first process
  RESTORE_CONTEXT(current_process);

  util::unreachable();
}

os::process::Pid os::process::exec(Program prog, size_t min_stack_size) {
  Process* proc;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    proc = root_process->find_first_unused(min_stack_size);

    // if this process is the last, create a new one to be the tail of the
    // process list
    if (proc->next == nullptr) {
      proc->next = (Process*)((uint8_t*)proc - min_stack_size - context_size -
                              sizeof(Process));
      // check if there is enough space for another process
      if ((uint8_t*)proc->next < memory::top_of_heap()) {
        proc->next = nullptr;
        return NullPid;
      }
      *proc->next = Process();
    }

    // set up initial context

    // program counter
    *((uint8_t*)proc - 1) = (uint16_t)prog & 0x00ff;
    *((uint8_t*)proc - 2) = ((uint16_t)prog & 0xff00) >> 8;

    proc->stack_top = (uint8_t*)proc - 1 - context_size;
    // clear registers
    memset(proc->stack_top + 1, 0, context_size - 2);

    // allow process to be run
    proc->state = Process::State::ready;
  }

  return proc;
}

os::process::Pid os::process::get_pid() {
  return current_process;
}

uint8_t* os::process::beginning_of_process_stacks() {
  Process* proc = root_process;
  while (proc->next) {
    proc = proc->next;
  }
  return (uint8_t*)proc;
}

/*! Returns the next process to be executed.
 *
 * Implemented as a round robin scheduler.
 */
Process* scheduler() {
  Process* p = current_process->next;
  if (p == nullptr) {
    p = root_process;
  }

  while (p != current_process) {
    if (p->state == Process::State::ready) {
      return p;
    }

    p = p->next ? p->next : root_process;
  }

  return current_process;
}

//! Switches to the next process as determined by the scheduler.
ISR(TIMER0_COMPB_vect, ISR_NAKED) {
  SAVE_CONTEXT(current_process);

  current_process = scheduler();

  RESTORE_CONTEXT(current_process);
}

Process* Process::find_first_unused(size_t min_stack_size) {
  for (Process* proc = this; proc; proc = proc->next) {
    if ((proc->state == State::unused) &&
        (proc->stack_size() == 0 || proc->stack_size() >= min_stack_size)) {
      return proc;
    }
  }

  util::unreachable();
}

size_t Process::stack_size() {
  if (next == nullptr) {
    return 0;
  }

  return ((size_t)this) - ((size_t)next) - sizeof(Process) - context_size;
}