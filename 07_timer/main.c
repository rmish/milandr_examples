#include "MDR32Fx.h"
#include "core_cm3.h"
#include "MDR32F9Qx_config.h"
#include "system_MDR32F9Qx.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include <MDR32F9Qx_timer.h>
#include "stdlib.h"
#include "stdio.h"

#define LED1 PORT_Pin_0 //определить нулевой вывод как LED1
#define LED2 PORT_Pin_1

void leds_init(void);
void timer_init(void);
void change_led(void);

int led_state;

int main()
{
  leds_init();
  timer_init();
  led_state=0;

  //бесконечный цикл
  while (1)
  {
    __NOP();
  }
}

void leds_init(void)
{
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

void timer_init(void)
{
  TIMER_CntInitTypeDef TIM1Init;
  // Включение тактирования таймера
  RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
  // Настройка делителя тактовой частоты для таймера
  TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv1);
  //Получение в структуру настроек по умолчанию
  TIMER_CntStructInit(&TIM1Init);
  //Задание предделителя тактовой частоты (раз в 8000 тактов
  // счётчик таймера увеличвается на 1)
  TIM1Init.TIMER_Prescaler = 8000;
  //Задание модуля счета (когда счётчик дойдёт до 1000, сработает прерывание)
  TIM1Init.TIMER_Period = 1000;
  // Настройка таймера с выбранными настройками предделителя тактовой
  // частоты и модуля счёта
  TIMER_CntInit(MDR_TIMER1, &TIM1Init);
  // Включение прерываний от таймера 1
  NVIC_EnableIRQ(TIMER1_IRQn);
  //Установка приоритета прерываний
  NVIC_SetPriority(TIMER1_IRQn, 0);
  TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ZERO, ENABLE);
  TIMER_Cmd(MDR_TIMER1, ENABLE);
}

void Timer1_IRQHandler(void)
{
  if (TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ZERO))
  {
    change_led();
    // Очистка флага прерывания в таймере для предотвращения
    // повторного вызова того же прерывания)
    TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ZERO);
  }
}

void change_led(void)
{
  if (led_state == 0)
  {
    PORT_SetBits(MDR_PORTC, LED1);
    PORT_ResetBits(MDR_PORTC, LED2);
    led_state = 1;
  }
  else
  {
    PORT_SetBits(MDR_PORTC, LED2);
    PORT_ResetBits(MDR_PORTC, LED1);
    led_state = 0;
  }
}

void exit(int code)
{
}
