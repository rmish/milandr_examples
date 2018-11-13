#include "MDR32Fx.h"
#include "core_cm3.h"
#include "MDR32F9Qx_config.h"
#include "system_MDR32F9Qx.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "stdlib.h"
#include "stdio.h"

#define LED1 PORT_Pin_0 //определить нулевой вывод как LED1
#define LED2 PORT_Pin_1

// диапазон 1-10
#define FRQMUL 1

void buttons_init(void);
void leds_init(void);
void init_frq(void);

int main()
{
  buttons_init();
  leds_init();
  init_frq();

  // выключить все светодиоды при старте
  PORT_ResetBits(MDR_PORTC, LED1 | LED2);

  //бесконечный цикл
  while (1)
  {
    PORT_SetBits(MDR_PORTC, LED1 | LED2);
    for (int i = 0; i < 500000; i++)
    {
      __NOP();
    }
    PORT_ResetBits(MDR_PORTC, LED1 | LED2);
    for (int i = 0; i < 500000; i++)
    {
      __NOP();
    }
  }
}

void buttons_init(void)
{
  //включить тактирование портов С, B, E
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);

  PORT_InitTypeDef PortInit; //объявление структуры PortInit

  //Инициализация порта кнопки SELECT (С2) на вход
  // направление передачи данных = вход
  PortInit.PORT_OE = PORT_OE_IN;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_PORT;
  // режим работы выводе =цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода= медленный
  PortInit.PORT_SPEED = PORT_SPEED_SLOW;
  // выбор вывода 2 для инициализации
  PortInit.PORT_Pin = PORT_Pin_2;
  //инициализация порта С заданными параметрами
  PORT_Init(MDR_PORTC, &PortInit);

  //Инициализация порта кнопки RIGHT (B6) и UP (B5) на вход
  // направление передачи данных = вход
  PortInit.PORT_OE = PORT_OE_IN;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_PORT;
  // режим работы выводе =цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода= медленный
  PortInit.PORT_SPEED = PORT_SPEED_SLOW;
  // выбор вывода 6 для инициализации
  PortInit.PORT_Pin = PORT_Pin_6 | PORT_Pin_5;
  //инициализация порта С заданными параметрами
  PORT_Init(MDR_PORTB, &PortInit);

  //Инициализация порта кнопки LEFT (E3) и DOWN (E1) на вход
  // направление передачи данных = вход
  PortInit.PORT_OE = PORT_OE_IN;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_PORT;
  // режим работы выводе =цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода= медленный
  PortInit.PORT_SPEED = PORT_SPEED_SLOW;
  // выбор вывода 3 для инициализации
  PortInit.PORT_Pin = PORT_Pin_3 | PORT_Pin_1;
  //инициализация порта С заданными параметрами
  PORT_Init(MDR_PORTE, &PortInit);
}

void leds_init(void)
{
  //включить тактирование портов С, B, E
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);

  PORT_InitTypeDef PortInit; //объявление структуры PortInit
  //Инициализация порта C на выход
  // направление передачи данных = Выход
  PortInit.PORT_OE = PORT_OE_OUT;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_PORT;
  // режим работы вывода = Цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода = медленный
  PortInit.PORT_SPEED = PORT_SPEED_SLOW;
  // выбор всех выводов для инициализации
  PortInit.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1);
  //инициализация заданными параметрами порта C
  PORT_Init(MDR_PORTC, &PortInit);
}

void init_frq(void)
{
  // Включение HSE внешнего кварцевого осцилятора
  RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  // Ждём вклчюения HSE осцилятора
  while (!(RST_CLK_HSEstatus() == SUCCESS))
    __NOP();
  // Выбор HSE осциллятора в качестве источника тактовых импульсов для CPU_PLL
  // и установка умножителя тактовой частоты CPU_PLL равного 0
  // Частота внешнего кварца равна 8 МГц Максимальная частота процессора 80 МГц ,
  // Устанавливаем источник тактирования и коэффициент умножения схемы PLL
  // первый параметр: источник, второй: коэффициент умножения -1
  RST_CLK_CPU_PLLconfig( RST_CLK_CPU_PLLsrcHSEdiv1, FRQMUL - 1);
  // Включение схемы PLL
  RST_CLK_CPU_PLLcmd(ENABLE);
  // Ожидание состояния готовности схемы PLL
  while (!(RST_CLK_CPU_PLLstatus() == SUCCESS))
    __NOP();
  //Если включение CPU_PLL прошло успешно
  // Установка установка предделителя частоты С3 CPU_C3_prescaler = 1
  RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
  // Включение схему умножения частоты PLL
  RST_CLK_CPU_PLLuse(ENABLE);
  // Использование для тактирования МК CPU_C3
  RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
}

void exit(int code)
{
}
