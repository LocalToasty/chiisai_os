#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <stdint.h>

namespace os {
namespace chrono {

using Timespan = uint32_t;

constexpr Timespan millisec = 1;
constexpr Timespan sec = 1000 * millisec;
constexpr Timespan min = 60 * sec;
constexpr Timespan hour = 60 * min;
constexpr Timespan day = 24 * hour;

//! Initializes the timing submodule.
void init();

//! Returns the time passed since initialisation.
Timespan time_since_init();

//! Pauses the current process for _at least_ ms milliseconds.
void delay_ms(Timespan ms);
}
}

#endif /* CHRONO_HPP */