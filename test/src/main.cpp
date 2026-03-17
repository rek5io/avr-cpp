#define F_CPU 16000000UL
#include "avr_io.hpp"

int main() {
    io::led::into_output();

    while (1) {
        io::led::set_high();
        _delay_ms(500);

        io::led::set_low();
        _delay_ms(500);
    }

    return 0;
}

/*
int main() {
    DDRB |= (1<<5);

    while (1) {
         PORTB |= (1<<5);
        _delay_ms(500);

         PORTB &= ~(1<<5);
        _delay_ms(500);
    }

    return 0;
}
*/
