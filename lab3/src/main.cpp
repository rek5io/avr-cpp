#define F_CPU 16000000UL
#include "../../avr_io.hpp"

namespace io = atmega_328p::io;

struct Display {
    static uint8_t digits[4];
    static uint8_t max_pos;
    static uint8_t pos;

    static auto init_gpio() -> void {
        io::registers::D::into_output();
        io::registers::C::into_output();
    }

    static auto drive() -> void {
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

        uint8_t next = SEG_MAP[digits[pos]];
        uint8_t select = DIG_SELECT[pos];
        io::registers::C::set_port(0x00);
        io::registers::D::set_port(next);
        io::registers::C::set_port(select);

        pos = pos >= max_pos ? 0 : pos + 1; 
    }

    static auto set_num(uint16_t num) -> void {
        digits[0] = num % 10; num /= 10;
        digits[1] = num % 10; num /= 10;
        digits[2] = num % 10; num /= 10;
        digits[3] = num % 10;
    }

    static auto set_max_pos(uint8_t n_pos) -> void {
        max_pos = n_pos > 3 ? 3 : n_pos;
    }

    static auto off() -> void {
        io::registers::D::set_port(0x00);
    }
};

uint8_t Display::digits[4] = { 0, 0, 0, 0 };
uint8_t Display::max_pos = 0;
uint8_t Display::pos = 0;

#define T_START 0

volatile uint16_t secs = T_START;
volatile bool count = false;

auto main() -> int {
    io::registers::B::into_input_pullup();
  
    Display::init_gpio();
    Display::set_num(T_START);
    Display::set_max_pos(3);

    atmega_328p::timers::Timer1::init(
        time::TimeUnit::secs(1),
        [](){
            if (count) {
                secs--;
                Display::set_num(secs);
            }

            if (secs == 0) {
                count = false;
            }
        }
    );

    uint8_t sw_last = 0xff;

    while (1) {
        uint8_t sw_current = io::registers::B::get_pin();
        uint8_t sw_pressed = (sw_last & ~sw_current);
        sw_last = sw_current;
      
        if (sw_pressed & (1 << PB1) && !count) {
            if (secs != T_START) {
                count = true;
            }
        }

        if (sw_pressed & (1 << PB2) && !count) {
            secs += 1;
            Display::set_num(secs);
        }
        
        if (sw_pressed & (1 << PB3) && !count) {
            secs += 10;
            Display::set_num(secs);
        }

        if (sw_pressed & (1 << PB0)) {
            count = false;
            secs = T_START;
            Display::set_num(secs);
        }

        Display::drive();
        _delay_ms(5);
    }
}
