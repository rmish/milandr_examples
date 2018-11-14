#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"
#include <stdbool.h>
#include <stdio.h>

#define delay(T) for(i = T; i > 0; i--)
int i;
// коэффициент для преобразования кода с АЦП в вольты (подбирается)
#define SCALE 1240

// Необходимые для инициализации структуры
// Самого АЦП
ADC_InitTypeDef ADC;
// Нужного нам канала
ADCx_InitTypeDef ADC1;

void ADCInit()
{
  // включение тактирования АЦП
  RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_ADC, ENABLE);
  // заполнение структуры с настройкми АЦП значениями по умолчанию
  ADC_StructInit(&ADC);
  // и инициализация АЦП этими значениями
  ADC_Init(&ADC);
  // заполнение структуры с настройками канала АЦП значениями по умолчанию
  ADCx_StructInit(&ADC1);
  // ВЫбираем канал 7 (порт PD7).
  // Перемычку входа АЦП XP2 нужно переставить в положение TRIM.
  ADC1.ADC_ChannelNumber = ADC_CH_ADC7;
  // настройка 7-го канала АЦП.
  ADC1_Init(&ADC1);
  // Включение и выставление наивысшего приоритета прерыванию от АЦП
  // в настройках NVIC контроллера
  NVIC_EnableIRQ(ADC_IRQn);
  NVIC_SetPriority(ADC_IRQn, 0);
  // включение прерывания от АЦП по завершению преобразования.
  ADC1_ITConfig(ADCx_IT_END_OF_CONVERSION, ENABLE);
  // включение работы АЦП1
  ADC1_Cmd(ENABLE);
}

bool conInProgress;
unsigned int rawResult;
unsigned char channel;
float result;

void ADC_IRQHandler()
{
  // если прерывание по завершению преобразования, обрабатываем результат
  if (ADC_GetITStatus(ADC1_IT_END_OF_CONVERSION))
  {
    // читаем регистр с результатами преобразования
    rawResult = ADC1_GetResult();
    // Получение номера канала, завершившего преобразование
    // биты 16..20 регистра
    channel = (rawResult & 0x1F0000) >> 16;
    // оставляем первые 12 бит регистра результата,
    // в которых содержится сам результат преобразвоания
    rawResult &= 0x00FFF;
    // преобразуем результат в вольты
    result = (float) rawResult / (float) SCALE;
    printf("Напряжение на переменном резистора %fВ (канал АЦП %i)\n",
        result,channel);
    fflush(stdout);
    conInProgress = false;
    // снимаем флаг ожидания прерывания АЦП
    NVIC_ClearPendingIRQ(ADC_IRQn);
  }
}
int main()
{
  // Настраиваем и запускаем АЦП
  ADCInit();
  while (1)
  {
    // После задержки если преобразование завершилось, запускаем заново
    delay(0xFFFF);
    if (!conInProgress)
    {
      ADC1_Start();
      conInProgress = true;
    }
  }
}

void exit(int code)
{
}
