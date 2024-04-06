/**
 * Пример для Arduino Nano.
 * 
 * Используем EEPROM для записи/чтения данных.
 * 
 * EEPROM (Electronically Erasable Read-Only Memory) — энергонезависимая память.
 * - ресурс памяти: 100 000 циклов записи на каждую ячейку памяти.
 * - размер памяти: 1024 байт.
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdint.h>

#define LED_PIN PB5 // PB5(D13)

// Переменная размещается в EEPROM памяти
// Переменной задается значение по умолчанию. Все значения по умолчанию для переменных EEPROM хранятся в файле .eep
// "Upload EEPROM" в PlatformIO создает файл .eep и загружает его значения.
uint8_t EEMEM eemem_var = 1;

int main(void)
{
  DDRB |= (1<<LED_PIN); // Настройка PB5 на выход

  uint8_t byte_value = 0;
  uint16_t word_value = 0;
  uint8_t data[10];
  uint8_t eemem_value = 0;

  eeprom_update_byte((uint8_t*)10, 0); // Читает данные перед записью, и не записывает их если данные совпадают.

  byte_value = eeprom_read_byte((uint8_t*)10); // Читаем байт
  word_value = eeprom_read_word((uint16_t*)20); // Читаем слово
  eeprom_read_block(&data, (const void*)30, 10); // Читаем блок данных
  eemem_value = eeprom_read_byte(&eemem_var); // Читаем значение переменной

  if (byte_value > 0) {
    PORTB |= (1<<LED_PIN);
  } else {
    PORTB &= ~(1<<LED_PIN);
  }

  while (1) {}
}