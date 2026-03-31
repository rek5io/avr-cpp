#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

namespace time {
    struct TimeUnit {
        uint64_t duration;

        static TimeUnit micros(uint32_t duration) {
            return TimeUnit {
                .duration = (uint64_t)duration,
            };
        }

        static TimeUnit millis(uint32_t duration) {
            return TimeUnit {
                .duration = (uint64_t)duration * 1000UL,
            };
        }

        static TimeUnit secs(uint16_t duration) {
            return TimeUnit {
                .duration = (uint64_t)duration * 1000000UL,
            };
        }
    }; 
}

namespace atmega_328p {
    namespace timers {
        struct Timer1 {
            static void (*callback)();

            inline static void set_callback(void (*cb)()) {
                callback = cb;
            }

            static void init(time::TimeUnit unit, void (*cb)()) {
                cli();

                callback = cb;

                if (callback == nullptr) {
                    sei();
                    return;
                }

                uint64_t time_us = unit.duration;
                uint16_t prescaler_val = 1;
                uint16_t prescaler_bits = (1 << CS10);

                if (time_us < (uint64_t)65536 * 1 * 1000000UL / F_CPU) {
                    prescaler_val = 1;
                    prescaler_bits = (1 << CS10);
                } else if (time_us < (uint64_t)65536 * 8 * 1000000UL / F_CPU) {
                    prescaler_val = 8;
                    prescaler_bits = (1 << CS11);
                } else if (time_us < (uint64_t)65536 * 64 * 1000000UL / F_CPU) {
                    prescaler_val = 64;
                    prescaler_bits = (1 << CS11) | (1 << CS10);
                } else if (time_us < (uint64_t)65536 * 256 * 1000000UL / F_CPU) {
                    prescaler_val = 256;
                    prescaler_bits = (1 << CS12);
                } else {
                    prescaler_val = 1024;
                    prescaler_bits = (1 << CS12) | (1 << CS10);
                }

                uint64_t ocr = (time_us * F_CPU) / (prescaler_val * 1000000UL);

                if (ocr > 65535) {
                    OCR1A = 65535;
                } else {
                    OCR1A = ocr - 1;
                }

                TCCR1A = 0;
                TCCR1B = (1 << WGM12);
                TIMSK1 = (1 << OCIE1A);
                TCCR1B |= prescaler_bits;

                sei();
            }
        };

        void (*Timer1::callback)() = nullptr;

        ISR(TIMER1_COMPA_vect) {
            if (Timer1::callback) {
                Timer1::callback();
            }
        }
    }

    namespace interrupts {
        enum TriggerMode : uint8_t {
            LowLevel,
            Change,
            FallingEdge,
            RisingEdge
        };

        template<typename P>
        struct IInteruptPin { 
            inline static void set_callback(void (*cb)()) {
                P::callback = cb;
            }

            static void init(TriggerMode mode, void (*cb)()) {
                cli();

                P::callback = cb;

                if (P::callback == nullptr) {
                    sei();
                    return;
                }

                switch (mode) {
                    case LowLevel:
                        P::trigger_on_low_level();
                        break;

                    case Change:
                        P::trigger_on_change();
                        break;

                    case FallingEdge:
                        P::trigger_on_falling_edge();
                        break;

                    case RisingEdge:
                        P::trigger_on_rising_edge();
                        break;
                }

                P::enable();

                sei();
            }
        };

        struct PinD2: IInteruptPin<PinD2> {
            static void (*callback)();

            inline static void enable() {
                EIMSK |= (1 << INT0);
            }

            inline static void trigger_on_low_level() {
                EICRA &= ~(1 << ISC01);
                EICRA &= ~(1 << ISC00);
            }

            inline static void trigger_on_change() {
                EICRA &= ~(1 << ISC01);
                EICRA |= (1 << ISC00);
            }

            inline static void trigger_on_falling_edge() {
                EICRA |= (1 << ISC01);
                EICRA &= ~(1 << ISC00);
            }

            inline static void trigger_on_rising_edge() {
                EICRA |= (1 << ISC01);
                EICRA |= (1 << ISC00);
            }
        };

        struct PinD3: IInteruptPin<PinD3> {
            static void (*callback)();

            inline static void enable() {
                EIMSK |= (1 << INT1);
            }

            inline static void trigger_on_low_level() {
                EICRA &= ~(1 << ISC11);
                EICRA &= ~(1 << ISC10);
            }

            inline static void trigger_on_change() {
                EICRA &= ~(1 << ISC11);
                EICRA |=  (1 << ISC10);
            }

            inline static void trigger_on_falling_edge() {
                EICRA |=  (1 << ISC11);
                EICRA &= ~(1 << ISC10);
            }

            inline static void trigger_on_rising_edge() {
                EICRA |=  (1 << ISC11);
                EICRA |=  (1 << ISC10);
            }
        };

        void (*PinD2::callback)() = nullptr;
        void (*PinD3::callback)() = nullptr;

        ISR(INT0_vect) {
            if (PinD2::callback) {
                PinD2::callback();
            }
        }

        ISR(INT1_vect) {
            if (PinD3::callback) {
                PinD3::callback();
            }
        }

        using d2 = PinD2;
        using d3 = PinD3;
    }

    namespace io {
        namespace registers {
            template<typename R>
            struct IRegister {
                inline static void set_port(uint8_t value) {
                    R::port() = value;
                }

                inline static uint8_t get_port() {
                    return R::port();
                }

                inline static void set_ddr(uint8_t value) {
                    R::ddr() = value;
                }

                inline static uint8_t get_ddr() {
                    return R::ddr();
                }

                inline static uint8_t get_pin() {
                    return R::pin();
                }

                inline static void into_output() {
                    R::ddr() = 0xff;
                }

                inline static void into_input() {
                    R::ddr() = 0x00;
                }

                inline static void into_input_floating() {
                    R::ddr() = 0x00;
                    R::port() = 0x00;
                }

                inline static void into_input_pullup() {
                    R::ddr() = 0x00;
                    R::port() = 0xff;
                }
            };

            struct B: IRegister<B> {
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

            struct C: IRegister<C> {
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

            struct D: IRegister<D> {
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

            inline static void toggle() {
                R::port() ^= (1<<BIT);
            }

            inline static uint8_t get() {
                return (R::pin() & (1<<BIT)) ? 1 : 0;
            }
        };

        using d0 = PinImpl<registers::D, 0>;
        using d1 = PinImpl<registers::D, 1>;
        using d2 = PinImpl<registers::D, 2>;
        using d3 = PinImpl<registers::D, 3>;
        using d4 = PinImpl<registers::D, 4>;
        using d5 = PinImpl<registers::D, 5>;
        using d6 = PinImpl<registers::D, 6>;
        using d7 = PinImpl<registers::D, 7>;
        using d8 = PinImpl<registers::B, 0>;
        using d9 = PinImpl<registers::B, 1>;
        using d10 = PinImpl<registers::B, 2>;
        using d11 = PinImpl<registers::B, 3>;
        using d12 = PinImpl<registers::B, 4>;
        using d13 = PinImpl<registers::B, 5>;
        using a0 = PinImpl<registers::C, 0>;
        using a1 = PinImpl<registers::C, 1>;
        using a2 = PinImpl<registers::C, 2>;
        using a3 = PinImpl<registers::C, 3>;
        using a4 = PinImpl<registers::C, 4>;
        using a5 = PinImpl<registers::C, 5>;
        using led = d13;
    }
}
