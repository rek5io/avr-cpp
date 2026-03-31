#define F_CPU 16000000UL
#include "avr_io.hpp"

namespace io = atmega_328p::io;
namespace timers = atmega_328p::timers;

int main() {
    io::registers::B::into_input_pullup();
    io::led::into_output();

    timers::Timer1::init(
        timers::TimeUnit::millis(250),
        [](){
            io::led::toggle();
        }
    );

    while (1) {
        _delay_ms(500);
    }

    return 0;
}
