#include "stepperMotor.h"

double abs_d(double l){
	if(l < 0) l = -l;
	return l;
}

void stepperMotor_Init(stepperMotor_t* sM,
		GPIO_TypeDef * ePort, GPIO_TypeDef *dPort,
		uint32_t ePin, uint32_t dPin,
		TIM_HandleTypeDef * pulseTimHandler, uint32_t pulseTimChannel,
		TIM_HandleTypeDef * stateUpdateTimHandler, uint32_t stateUpdateTimChannel,
		logicType_t eL, logicType_t dL,
		uint32_t mic)
{
	sM->enPort = ePort;
	sM->enPin = ePin;
	sM->dirPort = dPort;
	sM->dirPin = dPin;
	sM->pulseTimerHandler = pulseTimHandler;
	sM->pulseTimerChannel = pulseTimChannel;
	sM->stateUpdateTimerHandler = stateUpdateTimHandler;
	sM->stateUpdateTimerChannel = stateUpdateTimChannel;

	sM->stepCounter = 0;
	sM->settedPosition_Pulses = 0;
	sM->stepsPerMM = 0;

	sM->settedPosition = 0;
	sM->currentPosition = 0;
	sM->settedVelocity = 0;
	sM->currentVelocity = 0;
	sM->settedAcceleration = 0;
	sM->currentAcceleration = 0;
	sM->stepsWhenMotorHasToStartDeaccelerate = 0;
	sM->prevVelocity = 0;
	sM->isPostionReached = false;
	sM->isSlowing = false;

	sM->maxVelocity = DEFAULT_MAX_VELOCITY_MMS;
	sM->maxAcceleration = DEFAULT_MAX_ACCELERATION_MMS2;

	sM->mcuClock_Hz = DEFAULT_MCU_CLOCK_FREQ_HZ;
	sM->microstepping = mic;

	HW_logic_setLogic(&(sM->enLogic), eL);
	HW_logic_setLogic(&(sM->dirLogic), dL);

	HAL_GPIO_WritePin(sM->enPort, sM->enPin, sM->enLogic.inactiveState);
	HAL_GPIO_WritePin(sM->dirPort, sM->dirPin, sM->enLogic.inactiveState);

	stepperMotor_SetVelocity_rps(sM, sM->settedVelocity);
	stepperMotor_UpdateTimerStop(sM);
	stepperMotor_Stop(sM);
}

void stepperMotor_PulseFinishedCallback(stepperMotor_t* sM){

	if(sM->stepCounter >= sM->stepsWhenMotorHasToStartDeaccelerate && !sM->isSlowing){
		sM->prevVelocity = sM->settedVelocity;
		sM->settedVelocity = 0;
		sM->isSlowing = true;
		stepperMotor_UpdateTimerStart(sM);
	}

	if(sM->stepCounter >= sM->settedPosition_Pulses){
		sM->isPostionReached = true;
		sM->isSlowing = false;
		sM->stepCounter = 0;
		sM->settedPosition_Pulses = 0;
		sM->stepsWhenMotorHasToStartDeaccelerate = 0;
		sM->settedVelocity = sM->prevVelocity;
		stepperMotor_Stop(sM);
		stepperMotor_UpdateTimerStop(sM);
	}
	else{
		++sM->stepCounter;
	}
}

void stepperMotor_Start(stepperMotor_t * sM){
	HAL_TIM_PWM_Start_IT(sM->pulseTimerHandler, sM->pulseTimerChannel);
}

void stepperMotor_Stop(stepperMotor_t * sM){
	HAL_TIM_PWM_Stop_IT(sM->pulseTimerHandler, sM->pulseTimerChannel);
}

void stepperMotor_UpdateTimerStart(stepperMotor_t * sM){
	HAL_TIM_Base_Start_IT(sM->stateUpdateTimerHandler);
}

void stepperMotor_UpdateTimerStop(stepperMotor_t * sM){
	HAL_TIM_Base_Start_IT(sM->stateUpdateTimerHandler);
}

void stepperMotor_Update(stepperMotor_t * sM){

	if(abs_d(sM->currentVelocity - sM->settedVelocity) > NUMERIC_ERROR){
		if(sM->currentVelocity < sM->settedVelocity){
			sM->currentVelocity = sM->currentVelocity + sM->settedAcceleration/100.0;
		}
		else{
			sM->currentVelocity = sM->currentVelocity - sM->settedAcceleration/100.0;
		}
		if(abs_d(sM->currentVelocity) < NUMERIC_ERROR)sM->currentVelocity = 0;
	}
	else{
		sM->currentVelocity = sM->settedVelocity;
	}

	stepperMotor_SetVelocity_rps(sM, sM->currentVelocity);
}

void stepperMotor_SetPosition(stepperMotor_t * sM, double p, bool waitUntilPositionIsReached){

	sM->isPostionReached = false;

	if(p > 0){
		stepperMotor_SetRevoluteDirection(sM, STEPPER_FORWARD);
	}
	else{
		stepperMotor_SetRevoluteDirection(sM, STEPPER_BACKWARD);
		p = -p;
	}

	sM->settedPosition_Pulses = (uint32_t)(p*sM->stepsPerMM);

	uint32_t steps = stepperMotor_ConvVeloToFreq(sM, CUTOUT_SLOWING_VELO_TAIL_FACTOR * sM->settedVelocity * sM->settedVelocity/(2*sM->settedAcceleration));
	sM->stepsWhenMotorHasToStartDeaccelerate = sM->settedPosition_Pulses - steps;

	stepperMotor_Start(sM);
	__HAL_TIM_SET_COMPARE(sM->stateUpdateTimerHandler, sM->stateUpdateTimerChannel, 5);
	stepperMotor_UpdateTimerStart(sM);

	while(!sM->isPostionReached && waitUntilPositionIsReached);
}

void stepperMotor_SetVelocity_rps(stepperMotor_t * sM, double v_rps){

	uint32_t product;

	if(abs_d(v_rps) < NUMERIC_ERROR){
		product = 0;
	}
	else{
		product = (sM->mcuClock_Hz)/((uint32_t)(v_rps * BASIC_STEPS_PER_REV * (double)sM->microstepping));
	}

	uint32_t dividers[] = {5,59,599,5999,59999};

	for(uint8_t i = 0; i < 5; i++){
		if(product/dividers[i]  <  65535){
			sM->pulseTimerHandler->Instance->PSC = dividers[i];
			sM->pulseTimerHandler->Instance->ARR = (product/dividers[i]) - 1;
			__HAL_TIM_SET_COMPARE(sM->pulseTimerHandler, sM->pulseTimerChannel, ((product/dividers[i])/2) - 1);
			break;
		}
	}
}

uint32_t stepperMotor_ConvVeloToFreq(stepperMotor_t * sM, double v_rps){
	return (uint32_t)(v_rps * BASIC_STEPS_PER_REV * sM->microstepping);
}

double stepperMotor_ConvFreqToVelo(stepperMotor_t * sM, uint32_t freq){
	if(abs(freq) < NUMERIC_ERROR) return 0;
	return (double)(freq /((double)BASIC_STEPS_PER_REV * (double)sM->microstepping));
}

void stepperMotor_SetAcceleration(stepperMotor_t * sM, double a){
	if(a < 0) a = 0;
	else if(a > sM->maxAcceleration) a = sM->maxAcceleration;

	sM->settedAcceleration = a;
}

void stepperMotor_DisableMotor(stepperMotor_t * sM){
	HAL_GPIO_WritePin(sM->enPort, sM->enPin, sM->enLogic.activeState);
}

void stepperMotor_EnableMotor(stepperMotor_t * sM){
	HAL_GPIO_WritePin(sM->enPort, sM->enPin, sM->enLogic.inactiveState);
}

void stepperMotor_SetRevoluteDirection(stepperMotor_t * sM, stepperMotorDirection_t d){
	switch(d){
	case STEPPER_FORWARD:
		HAL_GPIO_WritePin(sM->dirPort, sM->dirPin, sM->enLogic.inactiveState);
		return;
	case STEPPER_BACKWARD:
		HAL_GPIO_WritePin(sM->dirPort, sM->dirPin, sM->enLogic.activeState);
		return;
	default:
		return;
	}
}

