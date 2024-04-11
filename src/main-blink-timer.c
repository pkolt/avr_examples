/**
 * Пример для Arduino Nano.
 * 
 * Одновременно мигаем разными светодиодами. 
 * Не используем функции задержек `_delay_ms()` из библиотеки AVR.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> // NULL definition

#define LED_RED_PIN PB3 // PB3(D11)
#define LED_YELLOW_PIN PB2 // PB2(D10)
#define LED_GREEN_PIN PB1 // PB1(D9)
#define LED_BLUE_PIN PB0 // PB0(D8)

volatile uint64_t count_ms; // МК никогда не превысит этот счетчик (миллионы лет)

typedef void (*timer_cb_t)(void); // Определяем тип функции обратного вызова

typedef struct {
    bool active; // Флаг вкл/выкл таймер.
    uint64_t time_finish; // Время до следующего вызова.
    timer_cb_t callback; // Функция для вызова.
    uint16_t delay_ms; // Задержка в миллисекундах.
    int8_t repeat; // Сколько раз вызывать функцию. Если задан -1 то будет вызываться бесконечно.
    uint8_t count; // Счетчик вызовов.
    bool immediately; // Нужно ли при запуске вызывать callback сразу или делать задержку в delay.
} timer_t;

timer_t timer_create(timer_cb_t callback, uint16_t delay_ms, int8_t repeat, bool immediately) {
    timer_t timer = {
        .active = false,
        .time_finish = 0,
        .callback = callback,
        .delay_ms = delay_ms,
        .repeat = repeat,
        .count = 0,
        .immediately = immediately,
    };
    return timer;
}

void timer_start(timer_t *timer) {
    timer->time_finish = timer->immediately ? 0 : count_ms + timer->delay_ms;
    timer->active = true;
}

void timer_stop(timer_t *timer) {
    timer->active = false;
    timer->count = 0;
}

void timer_next_tick(timer_t *timer) {
    if (timer->active && timer->time_finish <= count_ms) {
        if (timer->count < timer->repeat || timer->repeat == -1) {
            if (timer->count < timer->repeat) {
                timer->count++;
            }
            if (timer->callback != NULL) {
                timer->callback();
            }
            timer->time_finish = count_ms + timer->delay_ms;
        } else {
            timer->active = false; // Stop timer
        }
    }
}

ISR(TIMER0_COMPA_vect) {
    count_ms++;
}

void callback_red(void) {
    PORTB ^= (1<<LED_RED_PIN);
}

void callback_yellow(void) {
    PORTB ^= (1<<LED_YELLOW_PIN);
}

void callback_green(void) {
    PORTB ^= (1<<LED_GREEN_PIN);
}

void callback_blue(void) {
    PORTB ^= (1<<LED_BLUE_PIN);
}

int main(void) {
    DDRB |= (1<<LED_RED_PIN) | (1<<LED_YELLOW_PIN) | (1<<LED_GREEN_PIN); // Настраиваем пины на выход

    TIMSK0 |= (1<<OCIE0A); // Включить прерывание при совпадении для Timer0
    TCCR0A |= (1<<WGM01); // Задаем режим CTC для Timer0
    OCR0A = 250; // Задаем значение для регистра совпадения: T(250) = 4 us * 250 = 1000 us = 1 ms

    sei(); // Разрешаем прерывания

    TCCR0B |= (1<<CS01) | (1<<CS00); // Задаем предделитель = 64 (~1ms) (CS02=0, CS01=1, CS00=1)

    timer_t timer_red = timer_create(&callback_red, 300, -1, true);
    timer_t timer_yellow = timer_create(&callback_yellow, 500, -1, true);
    timer_t timer_green = timer_create(&callback_green, 1000, -1, true);
    timer_t timer_blue = timer_create(&callback_blue, 200, -1, false);

    timer_start(&timer_red);
    timer_start(&timer_yellow);
    timer_start(&timer_green);
    timer_start(&timer_blue);

    while(1) {
        timer_next_tick(&timer_red);
        timer_next_tick(&timer_yellow);
        timer_next_tick(&timer_green);
        timer_next_tick(&timer_blue);
    }
}
