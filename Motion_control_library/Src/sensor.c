#include "sensor.h"

void sensor_Init(sensor_t * s, GPIO_TypeDef * pPort, uint16_t pPin, logicType_t lg){
	s->port = pPort;
	s->pin = pPin;
	HW_logic_setLogic(&(s->logic), lg);
}

GPIO_PinState sensor_getState(sensor_t *s){
	return HAL_GPIO_ReadPin(s->port, s->pin);
}
