


#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include <stdint.h>
#include <stdio.h>
#include "mstn_clk.h"

//Rx Tx
void TransmitSymbol (uint8_t *symb) { 
 
    printf("Start transmit symbol...\n"); 
 
    PORT_SetBits(MDR_PORTA, PORT_Pin_0); 
 
    for(uint8_t i = 0; i < 8; i++) { 
        if(symb[i] == '.') 
            PORT_SetBits(MDR_PORTA, PORT_Pin_2); 
        else if(symb[i] == '-') 
            PORT_ResetBits(MDR_PORTA, PORT_Pin_2); 
        else
            break;
        Delay(200); 
    } 
    PORT_ResetBits(MDR_PORTA, PORT_Pin_0); 
    printf("End transmit symbol!\n"); 
    PORT_ResetBits(MDR_PORTA, PORT_Pin_2);
}

void ReceiveSymbol(uint8_t *symb) {
    printf("Start receive symbol!\n");
    
    uint8_t i = 0;
    
    while(PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_3))
    {
        if(PORT_ReadInputDataBit(MDR_PORTA, PORT_Pin_1) != 0) 
            symb[i] = '.';
        else 
            symb[i] = '-';
        i++;
        Delay(200);
    }
    printf("End receive symbol!\n");
    printf("Sign: %s \n", symb);
}