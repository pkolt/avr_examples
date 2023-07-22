# AVR

## Сборник примеров программ для AVR:

- [Blink](./src/main-blink.c)
- [Timer/Counter0](./src/main-timer0.c)

## Базовая информация (ATmega328P)

### Побитовые операции для управления регистрами

#### Установка бита

```c
int PORT = 0b00000000;
PORT |= (1 << 0); // Установить 0-й бит в значение равно 1
PORT |= (1 << 2); // Установить 2-й бит в значение равно 1
PORT |= (1 << 0) | (1 << 7) // Установить первый и последний бит в значение равно 1
```

#### Снятие бита

```c
int PORT = 0b11111111;
PORT &= ~(1 << 0); // Установить 0-й бит в значение равно 0
PORT &= ~((1 << 0) | (1 << 2)); // Установить 0-й и 2-й бит в значение равно 0
```

#### Инвертирование бита

```c
int PORT = 0b11111111;
PORT ^= 1 << 0; // Инвертировать 0-й бит
PORT ^= ((1 << 0) | (1 << 2)); // Инвертировать 0-й и 2-й биты
```

#### Проверка бита

```c
int PORT = 0b00000100;

if (0 == (PORT & (1 << 3))) { // Проверить что 3-й бит равен 0
    printf("bit is 0\n");
}

if (~PORT & (1 << 3)) { // Проверить что 3-й бит равен 0
    printf("bit is 0\n");
}

if (PORT & (1 << 2)) { // Проверить что 2-й бит равен 1
    printf("bit is 1\n");
}
```

#### Как сделать побитовые операции более читаемыми?

Вместо конструкции `(1 << bit)` используйте макрос `_BV()` он объявлен в файле AVR `avr/include/avr/sfr_defs.h`:

```c
// avr/include/avr/sfr_defs.h
#define _BV(bit) (1 << (bit))
```

Используйте функции объявленные в прилагаемой библиотеке `include/bitwise.h`.

### Порты ввода-вывода

Порты ввода-вывода предназначены считывания или подачи напряжения.

Перед использованием порт необходимо настроить с помощью регистра DDRx.

Регистры направления данных DDR (direct data register).

- **порт B** - DDRB (полный - 8 бит, PB0..PB7)
- **порт C** - DDRC (неполный - 7 бит, PC0..PC6)
- **порт D** - DDRD (полный - 8 бит, PD0..PD7)

Для настройки порта записать в регистр:

- **0** - input (порт способен измерять напряжение на входе как вольтметр)
- **1** - output (порт способен подавать напряжение на выход, зажечь светодиод)

```c
DDRB |= (1 << 5); // настройка пина PB5 на выход (используем 5-й бит т.к. нумерация порта PB начинается с 0)
```

Управлять подачей напряжения на порт настроенный на выход можно с помощью регистра PORTx.

Для настройки порта записать в регистр:

- **0** - low (0V)
- **1** - high (5V)

## Приложение

### Время

| Название         | second | millisecond | microsecond | nanosecond    |
|------------------|--------|-------------|-------------|---------------|
| second (s)       | 1      | 1 000       | 1 000 000   | 1 000 000 000 |
| millisecond (ms) | 0.001  | 1           | 1 000       | 1 000 000     |
| microsecond (us) | 1.0E-6 | 0.001       | 1           | 1 000         |
| nanosecond (ns)  | 1.0E-9 | 1.0E-6      | 0.001       | 1             |

### AVR Library

- `avr/interrupt.h`: этот модуль предоставляет функции для работы с прерываниями.
- `avr/pgmspace.h`: этот модуль предоставляет функции для работы с программируемой памятью.
- `avr/eeprom.h`: этот модуль предоставляет функции для работы с EEPROM.
- `avr/wdt.h`: этот модуль предоставляет функции для работы с таймером watchdog.
- `avr/sleep.h`: этот модуль предоставляет функции для работы с режимами сна.
- `avr/power.h`: этот модуль предоставляет функции для управления питанием микроконтроллера.
- `avr/usart.h`: этот модуль предоставляет функции для работы с UART.
- `avr/twi.h`: этот модуль предоставляет функции для работы с шиной I2C.
- `avr/spi.h`: этот модуль предоставляет функции для работы с шиной SPI.
- `avr/adc.h`: этот модуль предоставляет функции для работы с АЦП.
- `avr/dac.h`: этот модуль предоставляет функции для работы с ЦАП.
- `avr/math.h`: этот модуль предоставляет математические функции.
- `avr/stdlib.h`: этот модуль предоставляет стандартные функции, такие как malloc(), free(), strlen() и т. д.
- `avr/string.h`: этот модуль предоставляет функции для работы со строками.
- `avr/ctype.h`: этот модуль предоставляет функции для работы с типом данных char.
- `avr/stdio.h`: этот модуль предоставляет функции для работы с потоками ввода-вывода.
- `avr/stdarg.h`: этот модуль предоставляет функции для работы с переменным числом аргументов.
- `avr/stdbool.h`: этот модуль предоставляет тип данных bool.
- `avr/stddef.h`: этот модуль предоставляет определения для различных макросов.

## Дополнительно

- [Установить драйвер CH340](http://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html)
- [Установить PlatformIO для VSCode](https://platformio.org)
- [Документация PlatformIO - Arduino](https://docs.platformio.org/en/stable/frameworks/arduino.html)
- [Arduino Nano](https://docs.arduino.cc/hardware/nano)
