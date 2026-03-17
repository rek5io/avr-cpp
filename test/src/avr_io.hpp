#pragma once

#include <avr/io.h>
#include <util/delay.h>

namespace io {
    namespace regs {
        struct RegB {
            inline static volatile uint8_t& ddr() {
                return DDRB;
            }

            inline static volatile uint8_t& port() {
                return PORTB;
            }

            inline static volatile uint8_t& pin() {
                return PINB;
            }
        };

        struct RegC {
            inline static volatile uint8_t& ddr() {
                return DDRC;
            }

            inline static volatile uint8_t& port() {
                return PORTC;
            }

            inline static volatile uint8_t& pin() {
                return PINC;
            }
        };

        struct RegD {
            inline static volatile uint8_t& ddr() {
                return DDRD;
            }

            inline static volatile uint8_t& port() {
                return PORTD;
            }

            inline static volatile uint8_t& pin() {
                return PIND;
            }
        };
    }

    template<typename R, uint8_t BIT>
    struct PinImpl {
        inline static void into_output() {
            R::ddr() |= (1<<BIT);
        }

        inline static void into_input() {
            R::ddr() &= ~(1<<BIT);
        }

        inline static void into_input_floating() {
            R::ddr() &= ~(1<<BIT);
            R::port() &= ~(1<<BIT);
        }

        inline static void into_input_pullup() {
            R::ddr() &= ~(1<<BIT);
            R::port() |= (1<<BIT);
        }

        inline static void set_high() {
            R::port() |= (1<<BIT);
        }

        inline static void set_low() {
            R::port() &= ~(1<<BIT);
        }

        inline static void set(uint8_t value) {
            if (value) {
                R::port() |= (1<<BIT);
            } else {
                R::port() &= ~(1<<BIT);
            }
        }

        inline static uint8_t get() {
            return (R::pin() & (1<<BIT)) ? 1 : 0;
        }
    };

    using d0 = PinImpl<regs::RegD, 0>;
    using d1 = PinImpl<regs::RegD, 1>;
    using d2 = PinImpl<regs::RegD, 2>;
    using d3 = PinImpl<regs::RegD, 3>;
    using d4 = PinImpl<regs::RegD, 4>;
    using d5 = PinImpl<regs::RegD, 5>;
    using d6 = PinImpl<regs::RegD, 6>;
    using d7 = PinImpl<regs::RegD, 7>;
    using d8 = PinImpl<regs::RegB, 0>;
    using d9 = PinImpl<regs::RegB, 1>;
    using d10 = PinImpl<regs::RegB, 2>;
    using d11 = PinImpl<regs::RegB, 3>;
    using d12 = PinImpl<regs::RegB, 4>;
    using d13 = PinImpl<regs::RegB, 5>;
    using a0 = PinImpl<regs::RegC, 0>;
    using a1 = PinImpl<regs::RegC, 1>;
    using a2 = PinImpl<regs::RegC, 2>;
    using a3 = PinImpl<regs::RegC, 3>;
    using a4 = PinImpl<regs::RegC, 4>;
    using a5 = PinImpl<regs::RegC, 5>;
    using led = d13;
}
