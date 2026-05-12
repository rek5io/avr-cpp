#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "LCDI2C.h"
#include <stdio.h>

void adc_init() {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN)  | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC)) {

    }

    uint32_t adc = ADC;
    uint32_t mv = (adc * 5000UL) >> 10;

    return (uint16_t)mv;
}

void lcd_write_voltage(LCD_I2C *lcd, uint16_t ch0, uint16_t ch1) {
    lcd->goTo(0, 0);

    char buffer[32] = {0};
    sprintf(buffer, "0:%6d 1:%6d", ch0, ch1);
    lcd->writeText(buffer);
}

void lcd_write_status(LCD_I2C *lcd, uint8_t on) {
    lcd->goTo(0, 1);

    if (on) {
        lcd->writeText((char*)"light on  ");
    } else {
        lcd->writeText((char*)"light off ");
    }
}

static int16_t hist = 400;
static uint8_t on = 0;

int main() {
    DDRB = 0xff;
    PORTB = 0x00;

    adc_init();
    //0x27 - adres urzadzenia na I2C
    LCD_I2C lcd = LCD_I2C(0x27);
    _delay_ms(100);
    lcd.backLightOn();
    lcd.clear();

    while (1) {
        uint16_t ch0 = adc_read(0);
        uint16_t ch1 = adc_read(1);

        if ((ch1 >= ch0) && !on) {
            on = 1;
        }

        if ((((int16_t)ch0 - (int16_t)ch1) >= hist) && on) {
            on = 0;
        }

        lcd_write_voltage(&lcd, ch0, ch1);
        lcd_write_status(&lcd, on);

        PORTB = on ? (1 << PB5) : 0x00;

        _delay_ms(20);
    }
}
