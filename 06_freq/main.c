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
#define FRQMUL 5

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
    for (int i=0;i<500000;i++) {__NOP();}
    PORT_ResetBits(MDR_PORTC, LED1 | LED2);
    for (int i=0;i<500000;i++) {__NOP();}
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
  /* Включение HSE внешнего кварцевого резонатора */
  RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  if (RST_CLK_HSEstatus() == SUCCESS) /* Если HSE осциллятор включился и прошел текст*/
  {
    // Выбор HSE осциллятора в качестве источника тактовых импульсов для CPU_PLL
    // и установка умножителя тактовой частоты CPU_PLL равного 0
    // Частота внешнего кварца равна 8 МГц Максимальная частота процессора 80 МГц ,
    // RST_CLK_CPU_PLLconfig ( Источник тактирования PLL, Коэффициент умножения 1);
    RST_CLK_CPU_PLLconfig( RST_CLK_CPU_PLLsrcHSEdiv1, FRQMUL-1);
    /* Включение схемы PLL*/
    RST_CLK_CPU_PLLcmd(ENABLE);
    if (RST_CLK_CPU_PLLstatus() == SUCCESS)
    //Если включение CPU_PLL прошло успешно
    {
      /* Установка CPU_C3_prescaler = 2 */
      RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV2);
      /* Установка CPU_C2_SEL от CPU_PLL выхода вместо CPU_C1 такта*/
      RST_CLK_CPU_PLLuse(ENABLE);
      /* Выбор CPU_C3 такта на мультиплексоре тактовых импульсов микропроцессора (CPU clock
       MUX) */
      RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
    }
    else
    {
      while (1)
        ;
    }
  }
  else
  {
    while (1)
      ;
  }

}

void exit(int code)
{
}
