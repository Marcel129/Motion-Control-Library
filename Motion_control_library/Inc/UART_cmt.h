#ifndef UART_CMT
#define UART_CMT

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "usart.h"

#define RX_BUFFER_SIZE 200
#define TX_BUFFER_SIZE 200

typedef struct{
	uint8_t txBuffer [RX_BUFFER_SIZE];
	uint8_t rxBuffer [TX_BUFFER_SIZE];
	uint8_t rxTmpChar;
	uint8_t rxBuffCursor;
	bool msgReadyToRead;

	UART_HandleTypeDef* UART_Handler;
} UART_structure_t;

#define CMD_NAME_LENGHT_T 4
#define CMD_PARAMS_NUMBER_T 5

typedef struct{
	uint8_t cmdName[CMD_NAME_LENGHT_T];
	double params[CMD_PARAMS_NUMBER_T];
} command;

extern UART_structure_t box_main_UART;

void Communication_Init(UART_structure_t *, UART_HandleTypeDef *huart);
void Communication_ClearRxBuffer(UART_structure_t *);
void Communication_ClearTxBuffer(UART_structure_t *);
void Communication_SendMsg(UART_structure_t *, uint8_t * msg);

//return number of correnctly read params
int8_t Communication_DecodeMsg(UART_structure_t *, command *cmd);


#endif
