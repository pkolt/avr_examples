/**
 * Пример для Arduino Nano.
 *
 * Срабатывание прерывания по переполнению Timer/Counter1.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitwise.h"

#define LED_PIN 5 // PB5(D13)

ISR(TIMER1_OVF_vect)
{
    invert_bit(PORTB, LED_PIN); // Включить/Выключить LED
}

int main(void)
{
    set_bit(DDRB, LED_PIN); // Настройка PB5 на выход

    // Настройка делителя (TCCR1B - Timer/Counter1 Control Register B)
    // Задаем prescaler = 256 (прерывание будет срабатывать когда пройдет примерно 1.04 s)
    set_bit(TCCR1B, CS12);
    clear_bit(TCCR1B, CS11);
    clear_bit(TCCR1B, CS10);

    // TCNT1H and TCNT1L - Timer/Counter1
    TCNT1 = 0; // Обнуляем значение таймера (0-65535)

    // TIMSK1 - Timer/Counter1 Interrupt Mask Register
    // Включаем прерывание по переполнению
    set_bit(TIMSK1, TOIE1);

    // SREG - AVR Status Register
    set_bit(SREG, SREG_I); // Разрешить глобальные прерывания
    // sei(); // Альтернативный способ разрешить глобальные прерывания

    while (1)
    {
    }
}