#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

struct Display {
    uint16_t disp_num = 0;
    uint8_t max_pos = 0;
    uint8_t pos = 0;

    auto drive() -> void {
        static const uint16_t DIVS[4] = {1, 10, 100, 1000};
        static const uint8_t SEG_MAP[10] = {
            0b00111111, // 0
            0b00000110, // 1
            0b01011011, // 2
            0b01001111, // 3
            0b01100110, // 4
            0b01101101, // 5
            0b01111101, // 6
            0b00000111, // 7
            0b01111111, // 8
            0b01101111  // 9
        };

        /*
        PC0 – S1  
        PC1 – S2  
        PC2 – S3  
        PC3 – S4  
        */

        PORTC = 0x00;
        PORTD = SEG_MAP[(disp_num / DIVS[this->pos]) % 10];
        PORTC = (1 << (3 - this->pos));

        this->pos++;
        if (this->pos > this->max_pos) {
            this->pos = 0;
        }

        _delay_ms(2);
    }

    auto set_num(uint16_t num) -> void {
        if (num )
        this->disp_num = num;
    }

    auto set_max_pos(uint8_t pos) -> void {
        this->max_pos = pos > 3 ? 3 : pos;
    }
};

auto main() -> int {
    DDRD = 0xff;
    PORTD = 0x00;
    DDRC = 0xff;
    PORTC = 0x00;
    DDRB = 0x00;
    PORTB = 0xff;

    Display disp;
    disp.set_num(69);

    while (1) {
        disp.drive();
    }
}

