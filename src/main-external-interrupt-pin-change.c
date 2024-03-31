/**
 * Пример для Arduino Nano.
 * 
 * Внешние прерывания по изменению состояния пина (Pin Change Interrupt).
 * Прерывание происходит при изменении значения (с LOW на HIGH и наоборот).
 * 
 * Доступны на пинах PCINTx, делятся на 3 группы:
 * - PCI0 - пины PCINT[7:0];
 * - PCI1 - пины PCINT[14:8];
 * - PCI2 - пины PCINT[23:16];
 * 
 * Программа. Будем включать и выключать светодиод при нажатии на кнопку.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "bitwise.h"

#define LED_PIN PB5 // D13
#define BUTTON_PIN PB0 // PCINT0/PB0 (D8)

volatile bool is_interrupt_button = false;

ISR(PCINT0_vect) {
  cli(); // Выключить глобальные прерывания
  is_interrupt_button = true;
}

int main(void)
{
  set_bit(DDRB, LED_PIN); // Настройка PB5 на выход

  clear_bit(DDRB, BUTTON_PIN); // Настройка PCINT0/PB0 (D8) на вход
  set_bit(PORTB, BUTTON_PIN); // Подтягиваем пин PCINT0/PB0 (D8) к HIGH через встроенный резистор

  set_bit(PCICR, PCIE0); // Разрешить прерывания на группе контактов PCIE0
  set_bit(PCMSK0, PCINT0); // Разрешить прерывание на PCINT0

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