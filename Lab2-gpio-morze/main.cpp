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


void clk_CoreConfig(void);

//Функция инициализации кнопки SA4
void button_Init(void);
//Функция считывания текущего состояния кнопки SA4
uint8_t button_State(void);

//Функция инициализации светодиода VD7
void led_Init(void);
//Функция записи состояния (1:0) светодиода VD7
void led_Write(bool on_off);
void BlinkLED(uint32_t num, uint32_t del);
void IndicateError(void);

//Morze
uint8_t MorzeToChar (uint8_t *symb);
void ClearSymbol(uint8_t *symb);
void LedMorze(uint8_t *symbols);
void PinInitA(PORT_OE_TypeDef mode, uint16_t pin);
//Rx Tx
void TransmitSymbol (uint8_t *symb);
void ReceiveSymbol(uint8_t *symb);

//int main(int argc, char *argv[])
int main()
{
    /*
    printf("Usb cycle\n");
    while(USB_GetStatus() != PERMITTED);
    printf("USB ready\n");
    
    printf("[");
    for(int i=0;i<100;i++){
            printf("%d",i);
            printf(":");
    }
    printf("]\n");
    */
    //clk_CoreConfig();
    led_Init();
    button_Init();
    PinInitA(PORT_OE_OUT, PORT_Pin_2);
    PinInitA(PORT_OE_IN, PORT_Pin_1);
    PinInitA(PORT_OE_OUT, PORT_Pin_0);
    PinInitA(PORT_OE_IN, PORT_Pin_3);
    
    //printf("Buffer save\n");
    BlinkLED(5,300);
    //printf("end\n");
    
    //uint8_t state = 0;
    //uint32_t waitTime = 5;
    uint8_t counter = 0;
    uint8_t symbol_tx[8];
    uint8_t symbol_rx[8];
    uint8_t numbBit = 0;
    uint8_t tempValue = 255;

    
    while (1){


        /*
        //printf("Input waitTime (ms):\n");
        //scanf("%lu", &waitTime);



        printf("Buffer save\n");
        BlinkLED(4,10000);
        printf("end\n");
        */
        
        if (PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_3)) {
            ClearSymbol(symbol_rx);
            ReceiveSymbol(symbol_rx);
            LedMorze(symbol_rx);
            tempValue = MorzeToChar(symbol_rx);
            tempValue == 255 ? printf("Symbol is undefined! \n") : printf("Char: %c \n", tempValue);
        }
        // Read button
        while (!button_State()) {
            counter++;
            Delay(500);
            if (counter == 4) printf("DEBUG INFO: Completed char! \n");
        }
        // Define sign
        if (counter > 0) {
            if (counter == 1) {
                symbol_tx[numbBit] = '.';
                numbBit++;
                printf("DEBUG INFO: %s \n", symbol_tx);
            }
            else if (counter < 4) {
                symbol_tx[numbBit] = '-';
                numbBit++;
                printf("DEBUG INFO: %s \n", symbol_tx);
            }
            else{
                for(numbBit;numbBit>8;--numbBit){
                   symbol_tx[numbBit] = 'x';
                }
            }
            // If end symbol
            if (numbBit == 8) {
                numbBit = 0;
                TransmitSymbol(symbol_tx);
                tempValue = MorzeToChar(symbol_tx);
                tempValue == 255 ? printf("Symbol is undefined! \n") : printf("Char: %c \n", tempValue);
            }
        }
        if (counter != 0) {
            //printf("numbSymb: %d \n", numbBit);
        }
        counter = 0;
        
    }
    /*
    printf("Start cycle\n");
    while (1){
        //Считывание текущего состояния кнопки SA4
        state = button_State(); //Считать состояние SA4
        led_Write(!state); //Включить светодиод при нажатии SA4
        printf("%d",state);
    }
    */
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
void clk_CoreConfig(void) {
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
        SystemCoreClockUpdate();
}

void PinInitA(PORT_OE_TypeDef mode, uint16_t pin){
	PORT_InitTypeDef PORT_InitStructure;
	PORT_InitStructure.PORT_OE = mode;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_InitStructure.PORT_Pin = (pin);
	PORT_Init(MDR_PORTA, &PORT_InitStructure);
}



void LedMorze (uint8_t *symbols) {
    for (uint8_t i = 0; i < 8; ++i) {
        if (symbols[i] == '.') {
            BlinkLED(1,500);
            //Delay(500);
        }
        else if (symbols[i] == '-') {
            BlinkLED(1,1500);
        }
        else {
            led_Write(0);
            break;
        }
        led_Write(0);
        Delay(500);
    }
}