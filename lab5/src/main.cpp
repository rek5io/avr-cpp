#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

class Uart {
    public:
        static auto init() -> void {
            uint16_t ubrr = 51;
            UBRR0H = (ubrr >> 8);
            UBRR0L = ubrr;
            UCSR0B = (1 << RXEN0) | (1 << TXEN0);
            UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
        }

        static auto println() -> void {
            putc_('\r');
            putc_('\n');
        }

        static auto println(const char *s) -> void {
            print(s);
            Uart::println();
        }

        static auto print(const char *s) -> void {
            while (*s) {
                putc_(*s++);
            }
        }

        static auto putc_(char c) -> void {
            while (try_send_char(c)) {}
        }

        static auto try_read_line(char *out, uint16_t size, uint8_t echo) -> uint16_t {
            char c = 0;
            uint16_t len = 0;

            while (1) {
                if (try_recv_char(&c) == 0) {
                    if (c == '\n' || c == '\r') {
                        if (echo) {
                            Uart::println();
                        }
                        out[len] = '\0';
                        return len;
                    }

                    if (len < size - 1) {
                        if (echo) {
                            Uart::putc_(c);
                        }
                        out[len++] = c;
                    } else {
                        if (echo) {
                            Uart::println();
                        }
                        out[len] = '\0';
                        return len;
                    }
                } else {
                    if (len > 0) {
                        continue;
                    } else {
                        return 0;
                    }
                }
            }
        }

        static auto try_recv_char(char *c) -> uint8_t {
            if (UCSR0A & (1 << RXC0)) {
                *c = UDR0;
                return 0;
            }
            return 1;
        }

        static auto try_send_char(char c) -> uint8_t {
            if (UCSR0A & (1 << UDRE0)) {
                UDR0 = c;
                return 0;
            }
            return 1;
        }
};

auto main() -> int {
    DDRB = 0xff;
    PORTB = 0x00;

    Uart::init();

    unsigned int year = 0;
    unsigned int check_year = 0;

    while (1) {
        char line[256] = {0};
        char out[256] = {0};

        if (Uart::try_read_line(line, 255, 1) > 0) {
            if (sscanf(line, "set-year %d", &year) == 1) {
                sprintf(out, ">> year set to %u", year);
                Uart::println(out);
            } else if (sscanf(line, "check-year %u", &check_year) == 1) {
                if (check_year > year) {
                    sprintf(out, ">> bad year, got %u but starting from %u", check_year, year);
                } else {
                    check_year = year - check_year;

                    if (check_year < 18) {
                        sprintf(out, "Masz %u lat, młodziku", check_year);
                    } else if (check_year >= 18 && check_year < 25) {
                        sprintf(out, "Masz %u lat, młody dorosły czy coś", check_year);
                    } else if (check_year >=25 && check_year < 65) {
                        sprintf(out, "Masz %u lat, stary", check_year);
                    } else if (check_year >= 65) {
                        sprintf(out, "Masz %u lat, całkiem stary", check_year);
                    }
                }

                Uart::println(out);
            } else {
                sprintf(out, ">> got bad cmd: %s", line);
                Uart::println(out);
            }
        }

        PORTB ^= (1 << PB5);

        _delay_ms(100);
    }

    return 0;
}
