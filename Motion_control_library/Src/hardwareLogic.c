#include <hardwareLogic.h>

void HW_logic_setLogic(logic_t* l, logicType_t lt){
	switch(lt){
	case HIGH_IS_ACTVE:
		l->logicType = HIGH_IS_ACTVE;
		l->activeState = GPIO_PIN_SET;
		l->inactiveState = GPIO_PIN_RESET;
		return;
	case LOW_IS_ACTVE:
		l->logicType = LOW_IS_ACTVE;
		l->activeState = GPIO_PIN_RESET;
		l->inactiveState = GPIO_PIN_SET;
		return;
	}
}

logicType_t HW_logic_invertLogic(logic_t* l){
	if(l->logicType == HIGH_IS_ACTVE){
		HW_logic_setLogic(l, LOW_IS_ACTVE);
		return LOW_IS_ACTVE;
	}
	else{
		HW_logic_setLogic(l, HIGH_IS_ACTVE);
		return HIGH_IS_ACTVE;
	}
}
