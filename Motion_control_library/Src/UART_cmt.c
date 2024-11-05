#include "UART_cmt.h"

UART_structure_t box_main_UART;

HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == box_main_UART.UART_Handler->Instance){
		if(box_main_UART.rxTmpChar == '\n' || box_main_UART.rxTmpChar == '\r'){
			box_main_UART.msgReadyToRead = true;
		}
		else{
			box_main_UART.rxBuffer[box_main_UART.rxBuffCursor] = box_main_UART.rxTmpChar;
			box_main_UART.rxBuffCursor = ((++box_main_UART.rxBuffCursor)%RX_BUFFER_SIZE);
		}
		HAL_UART_Receive_IT(box_main_UART.UART_Handler, &(box_main_UART.rxTmpChar), 1);
	}
}

void Communication_Init(UART_structure_t * uartStr, UART_HandleTypeDef *huart){
	Communication_ClearRxBuffer(uartStr);
	Communication_ClearTxBuffer(uartStr);

	uartStr->UART_Handler = huart;
	HAL_UART_Receive_IT(uartStr->UART_Handler, &(uartStr->rxTmpChar), 1);
}

void Communication_ClearRxBuffer(UART_structure_t * uartStr){
	uartStr->rxTmpChar = 0;
	uartStr->rxBuffCursor = 0;
	uartStr->msgReadyToRead = false;
	for(uint8_t i = 0; i<RX_BUFFER_SIZE; i++){
		uartStr->rxBuffer[i] = 0;
	}
}

void Communication_ClearTxBuffer(UART_structure_t * uartStr){
	for(uint8_t i = 0; i<TX_BUFFER_SIZE; i++){
		uartStr->txBuffer[i] = 0;
	}
}

int8_t Communication_DecodeMsg(UART_structure_t * uartStr, command *cmd){

	uint8_t *p = NULL, *p_prev = NULL, tmpBuff[20] = {0};
	int8_t paramIdx = 0;

	for(uint8_t i = 0; i<CMD_NAME_LENGHT_T; i++)cmd->cmdName[i] = 0;
	for(uint8_t i = 0; i<CMD_PARAMS_NUMBER_T; i++)cmd->params[i] = 0;

	p = strchr(uartStr->rxBuffer, ' ');
	strncpy(cmd->cmdName, uartStr->rxBuffer, 3);
	if(p == NULL) {
		Communication_ClearRxBuffer(uartStr);
		return 0;//no arguments
	}
	else if(p - uartStr->rxBuffer != 3){
		Communication_ClearRxBuffer(uartStr);
		return -1; //cmd name is not valid, abort
	}

	while(p != NULL){
		p_prev = p + 1;
		p = strchr( p + 1, ' ' );
		if(p == NULL){
			strcpy(tmpBuff, p_prev);
		}
		else{
			int8_t len = p - p_prev;
			strncpy(tmpBuff, p_prev, len);
		}
		cmd->params[paramIdx++] = atof(tmpBuff);

		if(paramIdx >= 5) {
			Communication_ClearRxBuffer(uartStr);
			return paramIdx;//too many arguments
		}

		for(uint8_t i = 0; i<20; i++) tmpBuff[i] = 0;
	}

	Communication_ClearRxBuffer(uartStr);
	return paramIdx;
}

void Communication_SendMsg(UART_structure_t * uartStr, uint8_t * msg){
	Communication_ClearTxBuffer(uartStr);
	uint16_t msgLen = strlen(msg);
	//prevention from too large messages
	if(msgLen > TX_BUFFER_SIZE) msgLen = TX_BUFFER_SIZE;
	strncpy(uartStr->txBuffer, msg, msgLen);
	HAL_UART_Transmit(uartStr->UART_Handler, uartStr->txBuffer, msgLen, 100);
}
