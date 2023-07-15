#include <avr/io.h>
#include <util/delay.h>
#include <bitwise.h>

// For Arduino Nano - PB5(D13)
#define PIN_PB5 5

const int LED_PIN = PIN_PB5;

int main(void)
{
  set_bit(DDRB, LED_PIN); // настройка PB5 на выход

  while (1)
  {
    invert_bit(PORTB, LED_PIN); // Инвертируем напряжение на PB5
    _delay_ms(1000);
  }
}