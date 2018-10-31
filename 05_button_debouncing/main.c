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
#define delay();  for (int i=0;i<10000;i++) __NOP();

static PORT_InitTypeDef PortInit; //объявление структуры PortInit

int main()
{
  int left = 1;
  int right = 1;
  //включить тактирование портов С, B, E
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);

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

  //Инициализация порта кнопки RIGHT (B6) на вход
  // направление передачи данных = вход
  PortInit.PORT_OE = PORT_OE_IN;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_PORT;
  // режим работы выводе =цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода= медленный
  PortInit.PORT_SPEED = PORT_SPEED_SLOW;
  // выбор вывода 6 для инициализации
  PortInit.PORT_Pin = PORT_Pin_6;
  //инициализация порта С заданными параметрами
  PORT_Init(MDR_PORTB, &PortInit);

  //Инициализация порта кнопки LEFT (E3) на вход
  // направление передачи данных = вход
  PortInit.PORT_OE = PORT_OE_IN;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_PORT;
  // режим работы выводе =цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода= медленный
  PortInit.PORT_SPEED = PORT_SPEED_SLOW;
  // выбор вывода 3 для инициализации
  PortInit.PORT_Pin = PORT_Pin_3;
  //инициализация порта С заданными параметрами
  PORT_Init(MDR_PORTE, &PortInit);

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

  // выключить все светодиоды при старте
  PORT_ResetBits(MDR_PORTC, LED1 | LED2);
  //PORT_ResetBits(MDR_PORTC, LED2);

  //бесконечный цикл
  while (1)
  {
    //если нажат SELECT
    if (PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_2) == Bit_RESET)
    {
      // выключить светодиоды
      PORT_ResetBits(MDR_PORTC, LED1 | LED2);
      right = 1;
      left = 1;
      printf("Select pressed\n");
    }
    if ((PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_6) == Bit_RESET) && right)
    {
      // выждать немного времени
      delay();
      // если не отпустили, включить светодиоды
      if (PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_6) == Bit_RESET)
      {
        PORT_SetBits(MDR_PORTC, LED2);
        right = 0;
        printf("Right pressed\n");
      }
    }
    if ((PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_3) == Bit_RESET) && left)
    {
      // выждать немного времени
      delay();
      // если не отпустили, включить светодиоды
      if (PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_3) == Bit_RESET)
      {
        PORT_SetBits(MDR_PORTC, LED1);
        left = 0;
        printf("Left pressed\n");
      }
    }
  }
}

void exit(int code)
{
}
