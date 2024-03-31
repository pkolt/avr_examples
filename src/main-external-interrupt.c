/**
 * Пример для Arduino Nano.
 * 
 * Внешние прерывания.
 * Внешние прерывания запускаются выводами INT0 и INT1 или любым из выводов PCINT23...0.
 * Раньше срабатывают прерывания с меньшим значением по вектору направления (например RESET = 0x000)
 * 
 * INT0/PD2(D2)
 * INT1/PD3(D3)
 * 
 * Программа. Будем включать и выключать светодиод при нажатии на кнопку.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "bitwise.h"

#define LED_PIN PB5 // D13
#define BUTTON_PIN PD2 // D2

volatile bool is_interrupt_button = false;

ISR(INT0_vect) {
  cli(); // Выключить глобальные прерывания
  is_interrupt_button = true;
}

int main(void)
{
  set_bit(DDRB, LED_PIN); // Настройка PB5 на выход

  clear_bit(DDRD, BUTTON_PIN); // Настройка INT0/PD2(D2) на вход
  set_bit(PORTD, BUTTON_PIN); // Подтягиваем пин INT0/PD2(D2) к HIGH через встроенный резистор

  // Настройка режима срабатывания INT0:
  // 00 - генерировать прерывание при LOW значении
  // 01 - генерировать прерывание при изменении значения (с LOW на HIGH и наоборот)
  // 10 - генерировать прерывание при изменении значения с HIGH на LOW
  // 11 - генерировать прерывание при изменении значения с LOW на HIGH
  set_bit(EICRA, ISC01);
  clear_bit(EICRA, ISC00);

  set_bit(EIMSK, INT0); // Разрешить прерывания на INT0

  sei(); // Включить глобальные прерывания

  while (1) {
    if (is_interrupt_button) {
      invert_bit(PORTB, LED_PIN); // Меняем значение на противоположное
      _delay_ms(1000); // Для борьбы с дребезгом кнопки
      sei(); // Включить глобальные прерывания
      is_interrupt_button = false;
    }
  }
}