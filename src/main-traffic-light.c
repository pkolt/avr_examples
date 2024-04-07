/**
 * Пример для Arduino Nano.
 * 
 * Светофор.
 * 
 * 1) Загорается зеленый сигнал.
 * 2) Загорается зеленый мигающий сигнал.
 * 3) Загорается желтый сигнал.
 * 4) Загорается красный сигнал.
 * 
 * В программе не используется стандартная функция _delay_ms(). Задержки реализованы с помощью таймера счетчика.
 * Таймер счетчик после запуска увеличивает значение переменной через каждую миллисекунду.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#define LED_RED_PIN PB3 // PB3(D11)
#define LED_YELLOW_PIN PB2 // PB2(D10)
#define LED_GREEN_PIN PB1 // PB1(D9)

volatile uint64_t timer_counter_ms; // МК никогда не превысит этот счетчик (миллионы лет)

ISR(TIMER0_COMPA_vect) {
    timer_counter_ms++;
}

#define GREEN_BIT 0
#define YELLOW_BIT 1
#define RED_BIT 2

#define LONG_DELAY 2000
#define SHORT_DELAY 400

typedef struct {
    const uint8_t state;
    const uint16_t delay;
} step_t;

const step_t STEPS[] = {
    {.state = (1<<GREEN_BIT), .delay = LONG_DELAY},
    {.state = 0, .delay = SHORT_DELAY},
    {.state = (1<<GREEN_BIT), .delay = SHORT_DELAY},
    {.state = 0, .delay = SHORT_DELAY},
    {.state = (1<<GREEN_BIT), .delay = SHORT_DELAY},
    {.state = 0, .delay = SHORT_DELAY},
    {.state = (1<<GREEN_BIT), .delay = SHORT_DELAY},
    {.state = (1<<YELLOW_BIT), .delay = LONG_DELAY},
    {.state = (1<<RED_BIT), .delay = LONG_DELAY},
};

void turn_led(uint8_t pin, bool value) {
    if (value) {
        PORTB |= (1<<pin);
    } else {
        PORTB &= ~(1<<pin);
    }
}

int main(void) {
    DDRB |= (1<<LED_RED_PIN) | (1<<LED_YELLOW_PIN) | (1<<LED_GREEN_PIN); // Настраиваем пины на выход

    TIMSK0 |= (1<<OCIE0A); // Включить прерывание при совпадении для Timer0
    TCCR0A |= (1<<WGM01); // Задаем режим CTC для Timer0
    OCR0A = 250; // Задаем значение для регистра совпадения: T(250) = 4 us * 250 = 1000 us = 1 ms

    sei(); // Разрешаем прерывания

    TCCR0B |= (1<<CS01) | (1<<CS00); // Задаем предделитель = 64 (~1ms) (CS02=0, CS01=1, CS00=1)

    uint64_t delay_finish = 0;
    uint8_t step_index = 0;
    uint8_t steps_size = sizeof(STEPS) / sizeof(STEPS[0]);
    bool is_start = true;

    while(1) {
        if (delay_finish <= timer_counter_ms) {
            step_t step = STEPS[step_index];
            turn_led(LED_GREEN_PIN, step.state & (1 << GREEN_BIT));
            turn_led(LED_YELLOW_PIN, step.state & (1 << YELLOW_BIT));
            turn_led(LED_RED_PIN, step.state & (1 << RED_BIT));
            if (is_start) {
                is_start = false;
                delay_finish = timer_counter_ms + step.delay;
            } else {
                is_start = true;
                step_index = step_index < (steps_size - 1) ? step_index + 1 : 0;
            }
        }
    }
}
