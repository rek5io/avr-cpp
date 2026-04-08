#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

struct Display {
    uint8_t digits[4] = { 0, 0, 0, 0 };
    uint8_t max_pos = 0;
    uint8_t pos = 0;

    auto init_gpio() -> void {
        DDRD = 0xff;
        PORTD = 0x00;
        DDRC = 0xff;
        PORTC = 0x00;
    }

    auto drive() -> void {
        static const uint8_t DIG_SELECT[4] = {
            0b1000,
            0b0100,
            0b0010,
            0b0001
        };

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

        uint8_t next = SEG_MAP[this->digits[this->pos]];
        uint8_t select = DIG_SELECT[this->pos];
        PORTC = 0x00;
        PORTD = next;
        PORTC = select;

        this->pos = this->pos >= this->max_pos ? 0 : this->pos + 1; 
    }

    auto set_num(uint16_t num) -> void {
        this->digits[0] = num % 10; num /= 10;
        this->digits[1] = num % 10; num /= 10;
        this->digits[2] = num % 10; num /= 10;
        this->digits[3] = num % 10;
    }

    auto set_max_pos(uint8_t pos) -> void {
        this->max_pos = pos > 3 ? 3 : pos;
    }

    auto off() -> void {
        PORTD = 0x00;  
    }
};

#define T_START 0

auto main() -> int {
    DDRB = 0x00;
    PORTB = 0xff;
  
    Display disp;
    disp.init_gpio();
    disp.set_num(T_START);
    disp.set_max_pos(1);

    uint16_t counter = 0;
    uint16_t secs = T_START;
    bool count = false;
    uint8_t sw_last = 0xff;

    while (1) {
        uint8_t sw_current = PINB;
        uint8_t sw_pressed = (sw_last & ~sw_current);
        sw_last = sw_current;
      
        disp.drive();

        if (sw_pressed & (1 << PB1) && !count) {
            if (secs != T_START) {
              count = true;
            }
        }
        
        if (sw_pressed & (1 << PB2) && !count) {
            secs += 1;
            disp.set_num(secs);
        }
        
        if (sw_pressed & (1 << PB3) && !count) {
            secs += 10;
            disp.set_num(secs);
        }

        if (sw_pressed & (1 << PB0)) {
            secs = T_START;
            disp.set_num(secs);
            count = false;
        }

        if (count) {
            if (counter >= 500) {
                counter = 0;
                secs -= 1;
                disp.set_num(secs);
                if (secs == 0) {
                    disp.off();
                    PORTD |= (1 << PD7);
                    _delay_ms(200);
                    PORTD &= ~(1 << PD7);
                    count = false;  
                }
            } else {
                counter += 1;  
            }
        }

        _delay_ms(2);
    }
}
