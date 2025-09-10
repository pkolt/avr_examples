/**
 * Пример работы с ИК-приемником TSOP4838 для Arduino Nano.
 * 
 * Описание:
 * Программа ожидает ИК-сигнал от пульта, использующего протокол NEC.
 * При получении определенной команды (адрес 0x00, команда 0x45), программа
 * инвертирует состояние светодиода, подключенного к пину PD4.
 *
 * ИК-приемник TSOP4838:
 * TSOP4838 - это модуль, принимающий ИК-сигналы, модулированные на частоте 38 кГц.
 * Важная особенность: его выходной сигнал (OUT) инвертирован.
 *  - Когда ИК-сигнал отсутствует, на выходе ВЫСОКИЙ уровень (HIGH).
 *  - Когда принимается ИК-импульс, на выходе НИЗКИЙ уровень (LOW).
 * Код в данном файле работает именно с этим инвертированным электрическим сигналом.
 *
 * Протокол NEC (описание электрического сигнала на выходе TSOP4838):
 *
 * 1. Стартовая последовательность:
 *    - Низкий уровень (LOW) в течение ~9 мс (начальный ИК-импульс).
 *    - Высокий уровень (HIGH) в течение ~4.5 мс (пауза).
 *
 * 2. Данные (32 бита, передаются младшим битом вперед - LSB first):
 *    - 8 бит: Адрес устройства.
 *    - 8 бит: Инвертированный адрес (~Адрес).
 *    - 8 бит: Команда.
 *    - 8 бит: Инвертированная команда (~Команда).
 *
 * 3. Кодирование битов (импульс + пауза):
 *    - Бит '0': Низкий уровень (~560 мкс) + Высокий уровень (~560 мкс).
 *    - Бит '1': Низкий уровень (~560 мкс) + Высокий уровень (~1.69 мс).
 *
 * 4. Завершающий импульс:
 *    - Низкий уровень (~560 мкс) для обозначения конца передачи.
 *
 * Пример данных для пульта (адрес 0x00, команда 0x45):
 * - Адрес: 0x00 (передается как 00000000)
 * - Инвертированный адрес: 0xFF (передается как 11111111)
 * - Команда: 0x45 (передается как 01000101)
 * - Инвертированная команда: 0xBA (передается как 10111010)
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#define LED_PIN PD4 // PD4(D4)
#define IR_RECEIVER_PIN PB0 // ICP1/PB0(D8)

// --- Константы для протокола NEC ---
// Длительности в тиках таймера (1 тик = 0.5 мкс при 16МГц и предделителе 8)
#define TICKS_9MS    18000 // 9ms
#define TICKS_4_5MS  9000  // 4.5ms
#define TICKS_560US  1120  // 560us
#define TICKS_1690US 3380  // 1.69ms

// Допуски для сравнения длительностей
#define TOLERANCE_9MS   200
#define TOLERANCE_4_5MS 200
#define TOLERANCE_BIT   150

// --- Глобальные переменные для ИК-приемника ---
volatile uint16_t ir_last_capture = 0;
volatile uint8_t ir_counter = 0;
volatile uint8_t ir_address = 0;
volatile uint8_t ir_address_inv = 0;
volatile uint8_t ir_command = 0;
volatile uint8_t ir_command_inv = 0;
volatile bool ir_finished = false;

void ir_reset() {
  ir_last_capture = 0;
  ir_counter = 0;
  ir_address = 0;
  ir_address_inv = 0;
  ir_command = 0;
  ir_command_inv = 0;
  ir_finished = false;

  // Захват по падающему фронту (ICES1 = 0) для обнаружения стартового импульса.
  TCCR1B &= ~(1 << ICES1);
}

void ir_next_edge(uint16_t current_capture) {
  ir_last_capture = current_capture;
  ir_counter++;
  // Инвертируем фронт для следующего захвата
  TCCR1B ^= (1 << ICES1);
}

// --- Функция для проверки длительностей ---
static inline bool is_duration_match(uint16_t duration, uint16_t target, uint16_t tolerance) {
    return (duration > (target - tolerance)) && (duration < (target + tolerance));
}

// Обработчик прерывания Input Capture (TIMER1_CAPT_vect)
ISR(TIMER1_CAPT_vect) {
  uint16_t current_capture = ICR1;
  uint16_t duration_ticks = current_capture - ir_last_capture;

  switch (ir_counter) {
    case 0: // Ожидание первого импульса (не используется, т.к. начинаем с 0)
      ir_next_edge(current_capture);
      break;
    
    case 1: // Стартовый импульс (9 мс, LOW)
      if (is_duration_match(duration_ticks, TICKS_9MS, TOLERANCE_9MS)) {
        ir_next_edge(current_capture);
      } else {
        ir_reset();
      }
      break;

    case 2: // Пауза после старта (4.5 мс, HIGH)
      if (is_duration_match(duration_ticks, TICKS_4_5MS, TOLERANCE_4_5MS)) {
        ir_next_edge(current_capture);
      } else {
        ir_reset();
      }
      break;

    // Обработка 32 битов данных (64 шага: 32 импульса + 32 паузы)
    case 3 ... 66: {
      // Нечетные шаги (3, 5, ..., 65) - это импульсы по ~560 мкс
      if ((ir_counter % 2) != 0) {
        if (is_duration_match(duration_ticks, TICKS_560US, TOLERANCE_BIT)) {
          ir_next_edge(current_capture);
        } else {
          ir_reset();
        }
      }
      // Четные шаги (4, 6, ..., 66) - это паузы, определяющие бит (0 или 1)
      else {
        bool is_bit_0 = is_duration_match(duration_ticks, TICKS_560US, TOLERANCE_BIT);
        bool is_bit_1 = is_duration_match(duration_ticks, TICKS_1690US, TOLERANCE_BIT + 150); // Увеличим допуск для '1'

        if (is_bit_0 || is_bit_1) {
          if (is_bit_1) {
            uint8_t bit_index = (ir_counter - 4) / 2;
            if (bit_index < 8) { // Адрес
              ir_address |= (1 << bit_index);
            } else if (bit_index < 16) { // Инвертированный адрес
              ir_address_inv |= (1 << (bit_index - 8));
            } else if (bit_index < 24) { // Команда
              ir_command |= (1 << (bit_index - 16));
            } else { // Инвертированная команда (bit_index < 32)
              ir_command_inv |= (1 << (bit_index - 24));
            }
          }
          ir_next_edge(current_capture);
        } else {
          ir_reset();
        }
      }
      break;
    }

    // Завершающий импульс
    case 67:
      if (is_duration_match(duration_ticks, TICKS_560US, TOLERANCE_BIT)) {
        ir_finished = true;
      } else {
        ir_reset();
      }
      // Не переключаем фронт, ждем сброса в main
      break;

    default:
      ir_reset();
      break;
  }
}

void led_invert() {
  PORTD ^= (1 << LED_PIN);
}

int main(void) {
  // Настройка LED_PIN на выход
  DDRD |= (1 << LED_PIN);

  // Настройка таймера Timer1
  TCCR1A = 0;
  // Предделитель на 8 (16МГц / 8 = 2МГц -> 0.5 мкс/тик)
  TCCR1B = (1 << CS11);

  // Включение прерывания по захвату (Input Capture)
  TIMSK1 |= (1 << ICIE1);

  // Начальная настройка для захвата
  ir_reset();

  // Включить глобальные прерывания
  sei();

  while (1) {
    if (ir_finished) {
      // Проверяем, что инвертированные байты соответствуют оригинальным
      if (ir_address == (uint8_t)~ir_address_inv && ir_command == (uint8_t)~ir_command_inv) {
        // Если адрес 0x00 и команда 0x45 (кнопка Power на многих пультах)
        if (ir_address == 0x00 && ir_command == 0x45) {
          led_invert();
        }
      }
      
      // Сбрасываем состояние для приема следующей команды
      ir_reset();
    }
  }
}
