/**
 * Пример для Arduino Nano.
 * Мигаем светодиодом на пине D13 (PB5) с интервалом в 1 секунду.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <bitwise.h>

#define LED_PIN 5 // PB5(D13)

int main(void)
{
  set_bit(DDRB, LED_PIN); // настройка PB5 на выход

  while (1)
  {
    invert_bit(PORTB, LED_PIN); // Инвертируем напряжение на PB5
    _delay_ms(1000);
  }
}