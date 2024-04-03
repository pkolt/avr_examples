/**
 * Пример для Arduino Nano.
 * 
 * Аналогово цифровой преобразователь (АЦП или ADC).
 * В ATmega328P АЦП имеет 10 разрядов (10 бит), интервал значений 0..1023
 * Преобразование запускается вручную единоразово, при наступлении события (см Trigger Source) или постоянно при завершении предыдущего (Auto Trigger Enable).
 * После завершения преобразования срабатывает прерывание (если включено на завершение преобразования).
 * 
 * Измеряем напряжение на фоторезисторе и показываем его величину включая светодиоды.
 * U >= 192 (горит красный)
 * U >= 384 (горит желтый)
 * U >= 576 (горит зеленый)
 * U >= 768 (горит синий)
 * 
 * Фоторезистор соединяем с резистором на 10K, к месту соединения подключаем A5.
 * Другую ногу фоторезистора подключаем к GND, а ногу резистора 10K к Vcc.
 */

#include <avr/io.h>
#include <util/delay.h>

#define LED_RED_PIN PB3 // PB3(D11)
#define LED_YELLOW_PIN PB2 // PB2(D10)
#define LED_GREEN_PIN PB1 // PB1(D9)
#define LED_BLUE_PIN PB0 // PB0(D8)

int main(void) {
  DDRB |= (1<<LED_RED_PIN) | (1<<LED_YELLOW_PIN) | (1<<LED_GREEN_PIN) | (1<<LED_BLUE_PIN); // Настройка LED на выход

  // ADC5/PC5 (A5) установлен на вход по умолчанию (настройка не требуется).

  // Опорное напряжение от AVcc (5V).
  ADMUX |= (1<<REFS0);

  // ADLAR = 0 (для 10 битного результата, ADC)
  // ADLAR = 1 (для 8 битного результата, ADCH)

  // MUX3...0 устанавливаем в 0b0101 (выбран ADC5)
  ADMUX |= (1<<MUX2) | (1<<MUX0);

  ADCSRA |= (1<<ADEN); // ADEN - ADC Enable

  // ADC делитель
  // Для точности 10 бит использовать частоту от 50 kHz до 200 kHz
  // Для точности < 10 бит использовать частоту больше 200 kHz
  // 16MHz / 128 = 125 kHz (делитель 128 подойдет для точности в 10 бит)
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC Prescaler = 128 (125 kHz) (ADPS2, ADPS1, ADPS0)

  // Trigger Source = Free Running mode. ADCSRB (ADTS2=0,ADTS1=0,ADTS0=0)

  while (1) {
    ADCSRA |= (1<<ADSC); // Запуск преобразования
    while(ADCSRA & (1<<ADSC)); // Ждем завершения преобразования (ADSC=0)
    // ADC - результат преобразования 10 бит
    // Включаем нужные светодиоды в зависимости от значения
    PORTB = (ADC >= 192) ? (PORTB | (1<<LED_RED_PIN)) : (PORTB & ~(1<<LED_RED_PIN));
    PORTB = (ADC >= 384) ? (PORTB | (1<<LED_YELLOW_PIN)) : (PORTB & ~(1<<LED_YELLOW_PIN));
    PORTB = (ADC >= 576) ? (PORTB | (1<<LED_GREEN_PIN)) : (PORTB & ~(1<<LED_GREEN_PIN));
    PORTB = (ADC >= 768) ? (PORTB | (1<<LED_BLUE_PIN)) : (PORTB & ~(1<<LED_BLUE_PIN));
  }
}