#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

/// 2.1

/*
//volatile uint8_t tab[2000];
 volatile int tab[2000];

int main() {
  int i=0;
 
  while (1) {
    tab[1]=i++;   
    _delay_ms(1000);
 
  }
}
*/


/// 2.2

/*
volatile float zmienna;
//volatile uint8_t zmienna;

int main() {
    zmienna=5;
    //zmienna+=41; //a jak zmieni się rozmiar programu po wykonaniu operacji 
 
    while (1) {
        _delay_ms(1000);
    }
}
*/

// -> float zmienna=5 tylko przypisuje statycznie, 
// +=41 importuje cały zestaw funckji do obsługi float



/// 4.1

/*
int main() {
    DDRD = 0xff;

    while (1) {
        PORTD = PORTD ? 0x00 : 0xff;
        _delay_ms(500);
    }
}
*/



/// 4.2

/*
int main() {
    DDRD = 0xff;
    PORTD = 0x01;
    uint8_t dir = 1;

    while (1) {
        if (dir) {
            PORTD = PORTD << 1;
        } else {
            PORTD = PORTD >> 1;
        }

        if (PORTD == 0x80) {
            dir = 0;
        } else if (PORTD == 0x01) {
            dir = 1;
        }

        _delay_ms(50);
    }
}
*/



/// 5.1

/*
int main() {
    DDRD = 0xff; // rejestr D jako wyjście
    PORTD = 0x00;
    DDRB = 0x00; // rejestr B jak wejście
    PORTB = 0xff; // pullup na rejestrze B

    while (1) {
        PORTD = (PINB & (1 << PB0)) ? 0x00 : 0xff;
       _delay_ms(1); 
    }
}
*/


/// 5.2

/*
int main() {
    DDRD = 0xff; // rejestr D jako wyjście
    PORTD = 0x00;
    DDRB = 0x00; // rejestr B jak wejście
    PORTB = 0xff; // pullup na rejestrze B
    uint8_t lock = 0;

    while (1) {
        uint8_t sw = !(PINB & (1 << PB1)); 

        if (sw && !lock) {
            lock = 1;
            PORTD = PORTD ? 0x00 : 0xff;
        }

        if (!sw && lock) {
            lock = 0;
        }

       _delay_ms(20); <- dodaj debounce 
    }
}
*/



/// 5.3

/*
int main() {
    DDRD = 0xff; // rejestr D jako wyjście
    PORTD = 0x00;
    DDRB = 0x00; // rejestr B jak wejście
    PORTB = 0xff; // pullup na rejestrze B
    uint8_t lock = 0;

    while (1) {
        if (PINB & (1 << PB0)) {
            PORTD &= ~(1 << PD0);
        } else {
            PORTD |= (1 << PD0);
        }

        uint8_t sw = !(PINB & (1 << PB1)); 

        if (sw && !lock) {
            lock = 1;
            PORTD ^= (1 << PD1);
        }

        if (!sw && lock) {
            lock = 0;
        }

       _delay_ms(20); <- dodaj debounce 
    }
}
*/

/// 6.1


int main() {
    DDRD = 0xff; // rejestr D jako wyjście
    PORTD = 0x00;
    DDRB = 0x00; // rejestr B jak wejście
    PORTB = 0xff; // pullup na rejestrze B

    uint8_t sw_last = 0xff;
    uint8_t leds = 0x00;
    bool state = true;

    while (1) {
        uint8_t sw_current = PINB;
        uint8_t sw_pressed = (sw_last & ~sw_current);
        sw_last = sw_current;

        if (sw_pressed & (1 << PB0)) {
            leds = leds >> 1;
        }

        if (sw_pressed & (1 << PB1)) {
            leds = 1 | (leds << 1);
        }

        if (sw_pressed & (1 << PB2)) {
            state = true;
        }

        if (sw_pressed & (1 << PB3)) {
            state = false;
        }

        PORTD = state ? leds : 0x00;

       _delay_ms(20); 
    }
}

