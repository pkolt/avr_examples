#include <avr/io.h>
#include <util/delay.h>

// For Arduino Nano - PB5(D13)
const int PIN_PB5 = 5;
const int LED_PIN = PIN_PB5;

int main(void)
{
  DDRB |= (1 << LED_PIN); // настройка PB5 на выход

  while (1)
  {
    PORTB ^= (1 << LED_PIN); // Инвертируем напряжение на PB5
    _delay_ms(1000);
  }
}