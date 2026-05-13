#include <avr/io.h>
#include <util/delay.h>
#include "LCDI2C.h"

class Adc {
    public:
        static auto init() -> void {
            ADMUX = (1 << REFS0);
            ADCSRA = (1 << ADEN)  | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
        }

        static auto read(uint8_t channel) -> uint16_t {
            ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
            ADCSRA |= (1 << ADSC);

            while (ADCSRA & (1 << ADSC)) {}

            uint32_t adc = ADC;
            uint32_t mv = (adc * 5000UL) >> 10;
            return (uint16_t)mv;
        }
};

auto main() -> int {
    DDRB = 0xff;
    PORTB = 0x00;

    Adc::init();
    //0x27 - adres urzadzenia na I2C
    LCD_I2C lcd = LCD_I2C(0x27);
    _delay_ms(100);
    lcd.backLightOn();
    lcd.clear();

    uint8_t refresh_counter = 0;
    int16_t hist = 400;
    uint8_t on = 0;

    while (1) {
        uint16_t ch0 = Adc::read(0);
        uint16_t ch1 = Adc::read(1);

        if ((ch1 >= ch0) && !on) {
            on = 1;
        }

        if ((((int16_t)ch0 - (int16_t)ch1) >= hist) && on) {
            on = 0;
        }

        if (refresh_counter >= 40) {
            lcd.goTo(0, 0);
            lcd.print("0:%4d 1:%4d", ch0, ch1);
            lcd.goTo(0, 1);
            lcd.print("light %s  ", on ? "on" : "off");
            refresh_counter = 0;
        } else {
            refresh_counter++;
        }

        PORTB = on ? (1 << PB5) : 0x00;

        _delay_ms(5);
    }

    return 0;
}
