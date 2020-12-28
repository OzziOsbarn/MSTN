

#include <stdlib.h>
#include <stdio.h>
//#include "stdint.h"
#include <inttypes.h>

#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_timer.h"

extern DMA_ChannelInitTypeDef DMA_InitStr;
extern DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
extern DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;
void DMA_IRQHandler(void)
{
  /* Reconfigure the inactive DMA data structure*/
  if((MDR_DMA->CHNL_PRI_ALT_SET &(1<<DMA_Channel_TIM1)) == (0<<DMA_Channel_TIM1))
  {
    DMA_AltCtrlStr.DMA_CycleSize = 32;
    DMA_Init(DMA_Channel_TIM1, &DMA_InitStr);
  }
  else if((MDR_DMA->CHNL_PRI_ALT_SET & (1<<DMA_Channel_TIM1)) == (1<<DMA_Channel_TIM1))
  {
    DMA_PriCtrlStr.DMA_CycleSize = 32;
    DMA_Init(DMA_Channel_TIM1, &DMA_InitStr);
  }
}

#define DMA_SELECT(CHANNEL)	(0x00000001<<(CHANNEL))
/* Private variables ---------------------------------------------------------*/
PORT_InitTypeDef PORT_InitStructure;
TIMER_CntInitTypeDef sTIM_CntInit;
DMA_ChannelInitTypeDef DMA_InitStr;
DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;
uint16_t Sine12bit[32] = {
                     2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                     3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                     599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main()
{
  RST_CLK_DeInit();
  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_DMA | RST_CLK_PCLK_PORTE),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_DAC),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2),ENABLE);

  /* Disable all interrupt */
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;

  /* Reset PORTE settings */
  PORT_DeInit(MDR_PORTE);

  /* Configure DAC pin: DAC2_OUT */

  /* Configure PORTE pin 0 */
  PORT_InitStructure.PORT_Pin   = PORT_Pin_0;
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
  PORT_Init(MDR_PORTE, &PORT_InitStructure);

  /* DMA Configuration */
  /* Reset all settings */
  DMA_DeInit();
  DMA_StructInit(&DMA_InitStr);
  /* Set Primary Control Data */
  DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)Sine12bit;
  DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)(&(MDR_DAC->DAC2_DATA));
  DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncHalfword;
  DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncNo;
  DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_PriCtrlStr.DMA_Mode = DMA_Mode_PingPong;
  DMA_PriCtrlStr.DMA_CycleSize = 32;
  DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;
  /* Set Alternate Control Data */
  DMA_AltCtrlStr.DMA_SourceBaseAddr = (uint32_t)Sine12bit;
  DMA_AltCtrlStr.DMA_DestBaseAddr   = (uint32_t)(&(MDR_DAC->DAC2_DATA));
  DMA_AltCtrlStr.DMA_SourceIncSize = DMA_SourceIncHalfword;
  DMA_AltCtrlStr.DMA_DestIncSize = DMA_DestIncNo;
  DMA_AltCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_AltCtrlStr.DMA_Mode = DMA_Mode_PingPong;
  DMA_AltCtrlStr.DMA_CycleSize = 32;
  DMA_AltCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_AltCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_AltCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;
  /* Set Channel Structure */
  DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
  DMA_InitStr.DMA_AltCtrlData = &DMA_AltCtrlStr;
  DMA_InitStr.DMA_Priority = DMA_Priority_Default;
  DMA_InitStr.DMA_UseBurst = DMA_BurstClear;
  DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;

  /* Init DMA channel TIM1*/
  DMA_Init(DMA_Channel_TIM1, &DMA_InitStr);

  /* Enable dma_req or dma_sreq to generate DMA request */
  MDR_DMA->CHNL_REQ_MASK_CLR = DMA_SELECT(DMA_Channel_TIM1);
  MDR_DMA->CHNL_USEBURST_CLR = DMA_SELECT(DMA_Channel_TIM1);

  /* Enable DMA_Channel_TIM1 */
  DMA_Cmd(DMA_Channel_TIM1, ENABLE);

  /* ADC Configuration */
  /* Reset all ADC settings */
  DAC_DeInit();
  /* DAC channel2 Configuration */
  DAC1_Init(DAC2_AVCC);
  /* DAC channel2 enable */
  DAC2_Cmd(ENABLE);

  /* TIMER1 Configuration */
  /* Time base configuration */
  TIMER_DeInit(MDR_TIMER1);
  TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);
  sTIM_CntInit.TIMER_Prescaler                = 0;
  sTIM_CntInit.TIMER_Period                   = 0xFF;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

  /* Enable DMA for TIMER1 */
  TIMER_DMACmd(MDR_TIMER1,(TIMER_STATUS_CNT_ARR), ENABLE);

  /* TIMER1 enable counter */
  TIMER_Cmd(MDR_TIMER1,ENABLE);

  /* Enable DMA IRQ */
  NVIC_EnableIRQ(DMA_IRQn);

  /* Infinite loop */
  while(1)
  {
  }
}