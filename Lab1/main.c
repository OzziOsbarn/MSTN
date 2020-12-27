/*
__][____][____][____][____][____][____][____][____][____][____][____][____][__
[____][____][____][____][____][____][____][____][____][____][____][____][____]
[___][__|/==================================================\
_____][_]¦<><><><><><><><><> 2020 TUSUR, FB <><><><><><><><><>¦            .------.____
__][____]¦> [@File          ]: main.c                         ¦         .-'       \ ___)
[____][_|¦> [@IDE           ]: NetBeans                       ¦      .-'         \\\
[___][__|¦> [@Version       ]: 1.0                            ¦   .-'        ___  \\)
__][____]¦> [@Date          ]: 27/12/2020                     ¦.-'          /  (\  |)
[____][_|¦                                                    ¦         __  \  ( | |
[___][__|¦> [@author        ]: E.U. Borisova, D.A. Osipov,    ¦        /  \  \__'| |
[__][___|¦>                    I.V. Zabaturina                ¦       /    \____).-'
__][____]¦> [@Mail          ]: stalkerkop@mail.ru             ¦     .'       /   |
[____][_|¦> [@Target board  ]: LDM: BB-K1986BE92QI-M8M        ¦    /     .  /    |
[___][__|¦>                                                   ¦._.'     / \/     |
__][____]\================>|/========\|<=====================/         /   \     |
[____][_|                 <|> Legend <|>                              /    /    _|_
[___][__|                   \________/                                \   /    /\ /\
__][____]Description:                                                  \ /    /__v__\
[____][_|   Led migalka na VD4 svetodiode                               '    |       |
[___][__| 80 and 32 MGh and do blink                                         |     .#|
__][____]                                                                    |#.  .##|
[____][_|                                                                    |#######| 
[___][__|                                                                    |#######| 
__][____]                                                            
[____][_|                      
[___][__|
__][____]
__][____][____][____][____][____][____][____][____][____][____][____][____][__
[____][____][____][____][____][____][____][____][____][____][____][____][____]
__][____][____][____][____][____][____][____][____][____][____][____][____][__
*/
/*+---	<INCLUDES>	---------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
//#include "stdint.h"
#include <inttypes.h>

#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "mstn_led.h"
#include "mstn_clk.h"
#include "mstn_usb.h"

/*+==================================================*/
/*+---	<DEFINES>	---------------------------------*/
#define LED1 PORT_Pin_0
/*+==================================================*/
/*+---	<FUNCTIONS DECLARATION>	---------------------*/

void BlinkLED(uint32_t num, uint32_t del);
void IndicateError(void);
//Функция инициализации кнопки SA4
void button_Init(void);
//Функция считывания текущего состояния кнопки SA4
uint8_t button_State(void);
//Функция инициализации светодиода VD7
void led_Init(void);
//Функция записи состояния (1:0) светодиода VD7
void led_Write(bool on_off);

void clk_CoreConfig80(void);
void clk_CoreConfig32(void);

//int main(int argc, char *argv[])
int main()
{
    printf("Usb cycle\n");
    while(USB_GetStatus() != PERMITTED);
    printf("USB ready\n");
    while (1){
        for(int i=0;i<100;i++){
            printf("%d",i);
            printf(" ");
        }


        uint8_t state = 0;
        uint32_t waitTime = 5;

        clk_CoreConfig80();
        printf("Start Init 80\n");
        led_Init();
        button_Init();

        printf("Buffer save\n");
        BlinkLED(4,10000);
        printf("end\n");

        //printf("Input waitTime (ms):\n");
        //scanf("%lu", &waitTime);

        printf("Start Init 32\n");
        clk_CoreConfig32();
        printf("Init 32\n");
        led_Init();
        button_Init();


        printf("Buffer save\n");
        BlinkLED(4,10000);
        printf("end\n");
    }
    return EXIT_SUCCESS;
}


/*+---	<FUNCTIONS DESCRIPTION>	---------------------*/
//
void BlinkLED(uint32_t num, uint32_t del){
	uint32_t cnt;
	for ( cnt = 0; cnt < num; cnt++)
	{
		Delay(del);
		led_Write(1);
		Delay(del);
                led_Write(0);
	}
}
//
void IndicateError(void){
	/*<<<>>> Switch LED3 on and off in case of error */
    BlinkLED(3,5000);
}

//Функция инициализации кнопки SA4
void button_Init(void){
//Создание структуры для инициализации порта
PORT_InitTypeDef PORT_InitStructure;
//Настройки порта: ввод, функция ввода/вывода, цифровой режим,
//минимальная скорость, Pin5
PORT_InitStructure.PORT_OE = PORT_OE_IN;
PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
PORT_InitStructure.PORT_Pin = (PORT_Pin_5);
PORT_Init(MDR_PORTD, &PORT_InitStructure);
}
//Функция считывания текущего состояния кнопки SA4
uint8_t button_State(void){
return PORT_ReadInputDataBit(MDR_PORTD, PORT_Pin_5);
}

//Функция инициализации светодиода VD7
void led_Init(void){
//Создание структуры для инициализации порта
PORT_InitTypeDef PORT_InitStructure;
//Настройки порта: вывод, функция ввода/вывода, цифровой режим,
//максимальная скорость, Pin2
PORT_InitStructure.PORT_OE = PORT_OE_OUT;
PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
PORT_InitStructure.PORT_Pin = (PORT_Pin_2);
PORT_Init(MDR_PORTC, &PORT_InitStructure);
}
//Функция записи состояния (1:0) светодиода VD7
void led_Write(bool on_off){
PORT_WriteBit(MDR_PORTC, PORT_Pin_2, on_off ? Bit_SET : Bit_RESET);
}

//Функция настройки тактовой частоты МК
void clk_CoreConfig80(void) {
	//Реинициализация настроек тактирования
	// Включить тактирование батарейного блока
	//и внутренние генераторы, все остальное сбросить
	RST_CLK_DeInit();
	//Включение тактирования от внешнего источника HSE (High Speed External)
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	//Проверка статуса HSE
	//if (RST_CLK_HSEstatus() == SUCCESS) /* Если HSE осциллятор включился 
	//if (RST_CLK_HSEstatus() == ERROR) while (1);
	while (RST_CLK_HSEstatus () != SUCCESS);
	//Настройка делителя/умножителя частоты CPU_PLL(фазовая подстройка частоты)
	/* Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) */
	/* PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);
	// RST_CLK_CPU_PLLconfig(div, mul);
	//Включение CPU_PLL
	//, но еще не подключать к кристаллу
	RST_CLK_CPU_PLLcmd(ENABLE);
	//Проверка статуса CPU_PLL
	//if (RST_CLK_CPU_PLLstatus() == SUCCESS) //Если включение CPU_PLL прошло успешно
	//if (RST_CLK_CPU_PLLstatus() == ERROR) while (1);
	while (RST_CLK_CPU_PLLstatus() != SUCCESS);
	
	/* Установка CPU_C3_prescaler = 2 */
	// Делитель CPU_C3_SEL ( CPU_C3_SEL = CPU_C2_SEL/2 )
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);

	//Коммутация выхода CPU_PLL на вход CPU_C3
	//На С2 идет с PLL, а не напрямую с С1 (CPU_C2_SEL = PLL)
	RST_CLK_CPU_PLLuse(ENABLE);
	//Выбор источника тактирования ядра процессора
	//CPU берет с выхода С3 (а может с выхода HSI,LSI,LSE) (HCLK_SEL = CPU_C3_SEL )
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	//Тактирование перифирии
	//Подача тактовой частоты на PORTC, PORTD
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
}
void clk_CoreConfig32(void) {
	//Реинициализация настроек тактирования
	// Включить тактирование батарейного блока
	//и внутренние генераторы, все остальное сбросить
	RST_CLK_DeInit();
	//Включение тактирования от внешнего источника HSE (High Speed External)
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	//Проверка статуса HSE
	//if (RST_CLK_HSEstatus() == SUCCESS) /* Если HSE осциллятор включился 
	//if (RST_CLK_HSEstatus() == ERROR) while (1);
	while (RST_CLK_HSEstatus () != SUCCESS);
	//Настройка делителя/умножителя частоты CPU_PLL(фазовая подстройка частоты)
	/* Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) */
	/* PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul4);
	// RST_CLK_CPU_PLLconfig(div, mul);
	//Включение CPU_PLL
	//, но еще не подключать к кристаллу
	RST_CLK_CPU_PLLcmd(ENABLE);
	//Проверка статуса CPU_PLL
	//if (RST_CLK_CPU_PLLstatus() == SUCCESS) //Если включение CPU_PLL прошло успешно
	//if (RST_CLK_CPU_PLLstatus() == ERROR) while (1);
	while (RST_CLK_CPU_PLLstatus() != SUCCESS);
	
	/* Установка CPU_C3_prescaler = 2 */
	// Делитель CPU_C3_SEL ( CPU_C3_SEL = CPU_C2_SEL/2 )
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);

	//Коммутация выхода CPU_PLL на вход CPU_C3
	//На С2 идет с PLL, а не напрямую с С1 (CPU_C2_SEL = PLL)
	RST_CLK_CPU_PLLuse(ENABLE);
	//Выбор источника тактирования ядра процессора
	//CPU берет с выхода С3 (а может с выхода HSI,LSI,LSE) (HCLK_SEL = CPU_C3_SEL )
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	//Тактирование перифирии
	//Подача тактовой частоты на PORTC, PORTD
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
}

/*
    _____________________
   /  __________________()
  /  /|_________________ 
 /  /_/________________()
/______________________
|_____________________() 
*/