// Тестовый пример с включением двух светодиодов на плате из отладочного
// комплекта

// Device Header
#include "MDR32Fx.h"
// Device Startup
#include "MDR32F9Qx_config.h"

// Определение констант для доступа к выводам порта С,
// к которым подключены светодиоды
#define VD1 0x002    // PC1
#define VD0 0x001    // PC0

int main(void)
{
  // Включаем тактирование порта C
  MDR_RST_CLK->PER_CLOCK |= 0x800000;
  // Переключение режима работы линий 0-1 порта С в "вывод"
  MDR_PORTC->OE |= 0x0003;
  // Аналоговый режим работы у линий 0-1 порта С
  MDR_PORTC->ANALOG |= 0x0003;
  // Длинный фронт (100нс)
  MDR_PORTC->PWR |= 0x05;
  // Вывод 1 на линию 0 порта С
  MDR_PORTC->RXTX |= VD0;
  // Вывод 1 на линию 1 порта С
  MDR_PORTC->RXTX |= VD1;
  while (1)
  {
    __NOP();
  }
}

void exit(int code)
{
}
