#define F_CPU 16000000UL
#include "avr_io.hpp"

namespace io = atmega_328p::io;

int main() {
    io::led::into_output();
    io::d2::into_input_pullup();
    io::d3::into_input_pullup();

    atmega_328p::timers::Timer1::init(
        time::TimeUnit::secs(2),
        [](){
            io::led::toggle();
        }
    );

    atmega_328p::interrupts::d2::init(
        atmega_328p::interrupts::TriggerMode::FallingEdge,
        [](){
            io::led::set_low();
        }
    );

    atmega_328p::interrupts::d3::init(
        atmega_328p::interrupts::TriggerMode::FallingEdge,
        [](){
            io::led::set_high();
        }
    );

    while (1) {}

    return 0;
}
