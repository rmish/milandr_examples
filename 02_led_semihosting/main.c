// Тестовый пример с включением двух светодиодов на плате из отладочного
// комплекта и использованием механизма semihosting

// Device Header
#include "MDR32Fx.h"
// Device Startup
#include "MDR32F9Qx_config.h"
// стандартные библиотеки для чтения из консоли и записи в неё.
#include "stdlib.h"
#include "stdio.h"

// Определение констант для доступа к выводам порта С,
// к которым подключены светодиоды
#define VD1 0x002    // PC1
#define VD0 0x001    // PC0

int main(void)
{
  int led;
  // Включаем тактирование порта C
  MDR_RST_CLK->PER_CLOCK |= 0x800000;
  // Переключение режима работы линий 0-1 порта С в "вывод"
  MDR_PORTC->OE |= 0x0003;
  // Аналоговый режим работы у линий 0-1 порта С
  MDR_PORTC->ANALOG |= 0x0003;
  // Длинный фронт (100нс)
  MDR_PORTC->PWR |= 0x05;
  // читаем с клавиатуры какой из светодиодов включить
  printf("Witch LED to turn on (0/1)?");
  // синхронизируем буферы для консоли (требуется при работе в Windows)
  fflush(stdout);
  scanf("%i",&led);
  // И включаем выбранный. Либо оба, если ввели чушь.
  switch (led)
  {
    case 0:
      MDR_PORTC->RXTX |= VD0;
      break;
    case 1:
      MDR_PORTC->RXTX |= VD1;
      break;
    default:
      MDR_PORTC->RXTX |= VD1;
      MDR_PORTC->RXTX |= VD0;
  }

  while (1)
  {
    __NOP();
  }
}

void exit(int code)
{
}
