#include <dcMotor.h>

void dcMotor_Init(dcMotor_t * dcM,
		GPIO_TypeDef *fPort, uint16_t fPin,
		GPIO_TypeDef *bPort ,uint16_t bPin,
		logicType_t lg){

	dcM->forwardPort = fPort;
	dcM->forwardPin = fPin;
	dcM->backwardPort = bPort;
	dcM->backwardPin = bPin;
	HW_logic_setLogic(&(dcM->logic), lg);
}

void dcMotor_start(dcMotor_t * dcM, dcMotor_moveDirection_t md){
	switch(md){
	case DCMOTOR_FORWARD:
		HAL_GPIO_WritePin(dcM->forwardPort, dcM->forwardPin, dcM->logic.activeState);
	case DCMOTOR_BACKWARD:
		HAL_GPIO_WritePin(dcM->backwardPort, dcM->backwardPin, dcM->logic.activeState);
	}
}

void dcMotor_stop(dcMotor_t * dcM){
	HAL_GPIO_WritePin(dcM->forwardPort, dcM->forwardPin, dcM->logic.inactiveState);
	HAL_GPIO_WritePin(dcM->backwardPort, dcM->backwardPin, dcM->logic.inactiveState);
}
