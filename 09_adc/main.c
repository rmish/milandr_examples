#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"
#include <stdbool.h>

#define delay(T) for(i = T; i > 0; i--)
int i;
#define SCALE 1252.5

ADC_InitTypeDef ADC;
ADCx_InitTypeDef ADC1;

void ADCInit()
{
  RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_ADC, ENABLE);
  ADC_StructInit(&ADC);
  ADC_Init(&ADC);
  ADCx_StructInit(&ADC1);
  ADC1.ADC_ChannelNumber = ADC_CH_ADC7; //Switch to TRIM!!!
  ADC1_Init(&ADC1);
  //Int
  NVIC_EnableIRQ(ADC_IRQn);
  NVIC_SetPriority(ADC_IRQn, 0);
  ADC1_ITConfig(ADCx_IT_END_OF_CONVERSION, ENABLE);
  ADC1_Cmd(ENABLE);
}

bool conInProgress;
unsigned int rawResult;
unsigned char channel;
float result;

void ADC_IRQHandler()
{
  if (ADC_GetITStatus(ADC1_IT_END_OF_CONVERSION))
  {
    rawResult = ADC1_GetResult();
    channel = (rawResult & 0x1F0000) >> 16;
    rawResult &= 0x00FFF;
    result = (float) rawResult / (float) SCALE;
    conInProgress = false;
    NVIC_ClearPendingIRQ(ADC_IRQn);
  }
}
int main()
{
  ADCInit();
  while (1)
  {
    delay(0xFFFF);
    if (!conInProgress)
    {
      ADC1_Start();
      conInProgress = true;
    }
  }
}

