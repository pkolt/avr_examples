/**
 * Пример для Arduino Nano.
 *
 * 8-битный Таймер-счетчик с PWM (Timer/Counter0)
 * Используется для прямого или обратного счета времени.
 * Представляет собой ячейку памяти хранящую 256 значений (0 - 255).
 * Через определенный промежуток времени таймер увеличивает значение в ячейке памяти.
 * При переполнении таймер автоматически обнуляется.
 * Частота работы микроконтроллера f = 16 MHz = 16 000 000 Hz
 *
 * T = 1/f; (время работы таймера без делителя)
 * T = 1 / 16 000 000 = 0.0000000625 s = 0.0625 us
 * T(255) = 0.0625 us * 255 = 15.93 us (время работы таймера до переполнения)
 *
 * Период делителя = 1 / (Частота счетчика / Делитель)
 * T = 1 / (16 000 000 / 8) = 0.5 us; T(255) = 127,5 us;
 * T = 1 / (16 000 000 / 64) = 4 us; T(255) = 1 020 us = 1 ms;
 * T = 1 / (16 000 000 / 256) = 16 us; T(255) = 4 080 us = 4 ms;
 * T = 1 / (16 000 000 / 1024) = 64 us; T(255) = 16 320 us = 16 ms;
 */

#include <avr/io.h>
#include <stdbool.h>
#include "bitwise.h"

#define LED_PIN 5 // PB5(D13)

int main(void)
{
  set_bit(DDRB, LED_PIN); // Настройка PB5 на выход

  // Настройка делителя (TCCR0B - Timer/Counter Control Register B)
  // Задаем prescaler = 1024
  set_bit(TCCR0B, CS02);
  clear_bit(TCCR0B, CS01);
  set_bit(TCCR0B, CS00);

  // Значение таймера (TCNT0 - Timer/Counter Register)
  TCNT0 = 0; // Обнуляем значение таймера (0-255)

  bool is_blocked = false; // Блокировка счетчика миллисекунд
  int counter_ms = 0;      // Счетчик для подсчета миллисекунд

  while (1)
  {
    // Когда таймер достигнет значения 16, пройдет примерно 1 ms (64 us * 16 = 1024 us = 1.02 ms)
    // Без использования блокировки светодиод будет мигать чуть быстрее чем 1 секунда,
    // это связано с тем что цикл while может сделать несколько проходов прежде чем таймер-счетчик сменит свое значение.
    // Воспринимайте таймер-счетчик как асинхронное устройство, связанное с программой лишь возможностью считывать значение.
    // Если в цикле while будет долго выполняющийся код, то может случиться так, что мы пропустим значение которое ждем в таймере-счетчике.
    if (TCNT0 == 16 && !is_blocked)
    {
      counter_ms++;
      is_blocked = true;
    }
    else
    {
      is_blocked = false;
    }

    if (counter_ms >= 1000)
    {
      invert_bit(PORTB, LED_PIN); // Включить/Выключить LED
      counter_ms = 0;             // Сбросить счетчик миллисекунд
    }
  }
}