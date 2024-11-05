#ifndef DCMOTOR_H
#define DCMOTOR_H

#include "gpio.h"
#include "hardwareLogic.h"

typedef enum {DCMOTOR_FORWARD, DCMOTOR_BACKWARD}
dcMotor_moveDirection_t;

typedef struct{

	GPIO_TypeDef * forwardPort, * backwardPort;
	uint16_t forwardPin, backwardPin;

	logic_t logic;

} dcMotor_t;

void dcMotor_Init(dcMotor_t * dcM,
		GPIO_TypeDef *,uint16_t, GPIO_TypeDef *,uint16_t,
		logicType_t);
void dcMotor_start(dcMotor_t *, dcMotor_moveDirection_t);
void dcMotor_stop(dcMotor_t *);

#endif
