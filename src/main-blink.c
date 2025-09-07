/**
 * Пример для Arduino Nano.
 * 
 * Мигаем светодиодом на пине D13 (PB5) с интервалом в 1 секунду.
 */

#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN PB5 // PB5(D13)

int main(void) {
  DDRB |= (1<<LED_PIN); // Настройка LED_PIN на выход

  while (1) {
    PORTB ^= (1<<LED_PIN); // Инвертируем напряжение на LED_PIN
    _delay_ms(1000); // Задержка в 1 секунду
  }
}
