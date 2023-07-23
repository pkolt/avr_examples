/**
 * Пример для Arduino Nano.
 *
 * Микроконтроллер после включения перейдет в спящий режим для экономия энергии.
 * При нажатии кнопки (соединении INT0/PD2/D2 с GNG) микроконтроллер выйдет из спящего режима и помигает светодиодом.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "bitwise.h"

#define LED_PIN 5       // PB5(D13)
#define INTERRUPT_PIN 2 // INT0/PD2(D2)

int main(void)
{
    set_bit(DDRB, LED_PIN); // Настройка PB5 на выход

    clear_bit(DDRD, INTERRUPT_PIN); // Настройка PD2 на вход (т.к. там кнопка)
    set_bit(PORTD, INTERRUPT_PIN);  // Подтягиваем PD2 к high

    // EICRA - External Interrupt Control Register A
    // Прерывание на INT0/PD2 будет сгенерировано при низком уровне (low)
    clear_bit(EICRA, ISC01);
    clear_bit(EICRA, ISC00);

    // EIMSK - External Interrupt Mask Register
    set_bit(EIMSK, INT0); // Включить внешнее прерывание на INT0

    // BOD - это аббревиатура от Brown-Out Detection.
    // Это функция, которая защищает микроконтроллер от повреждения из-за пониженного напряжения питания.
    // Если напряжение питания упадет ниже определенного порогового значения, BOD сработает и перезагрузит микроконтроллер.
    // Включен или выключен по умолчанию BOD зависит от установленных фьюзов в микроконтроллере.
    sleep_bod_disable();

    while (1)
    {
        sei(); // Разрешить глобальные прерывания

        // Режим Power-down Mode - это самый экономичный режим работы микроконтроллера ATmega328P.
        // В этом режиме все внутренние блоки микроконтроллера, за исключением таймера реального времени (RTC), отключаются.
        // В результате потребление энергии микроконтроллера составляет менее 1 мкА.

        // Микроконтроллер выйдет из режима Power-down Mode, когда произойдет событие, которое сгенерирует прерывание RTC.
        // Например, это может быть срабатывание таймера RTC, приход внешнего сигнала на вход INT0 или INT1,
        // изменение состояния шины SPI или I2C и т. д.
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
        // Функция sleep_mode() аналогична вызову нескольких функций:
        // - sleep_enable(); // разрешает перевод микроконтроллера в режим сна
        // - sleep_cpu(); // переводит микроконтроллер в режим сна
        // - sleep_disable(); // запрещает перевод микроконтроллера в режим сна

        // Попадем в это место при возникновении прерывания на INT0/PD2
        cli(); // Запретить глобальные прерывания

        for (int i = 0; i < 3; i++)
        {
            set_bit(PORTB, LED_PIN); // Включить светодиод на PB5
            _delay_ms(1000);
            clear_bit(PORTB, LED_PIN); // Выключить светодиод на PB5
            _delay_ms(1000);
        }
    }
}