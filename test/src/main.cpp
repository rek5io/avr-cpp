#define F_CPU 16000000UL
#include "avr_io.hpp"

namespace io = atmega_328p::io;

int main() {
    io::registers::B::into_input_pullup();
    io::led::into_output();

    atmega_328p::timers::Timer1::init(
        time::TimeUnit::millis(250),
        [](){
            io::led::toggle();
        }
    );

    while (1) {
        _delay_ms(500);
    }

    return 0;
}
