#ifndef INC_STEPPERMOTOR_H_
#define INC_STEPPERMOTOR_H_

#include <hardwareLogic.h>
#include "machineStates.h"
#include "gpio.h"
#include "tim.h"
#include "stdbool.h"
#include "math.h"

#define DEFAULT_MAX_ACCELERATION_MMS2 	2000
#define DEFAULT_MAX_VELOCITY_MMS 		2000

#define DEFAULT_MCU_CLOCK_FREQ_HZ 		8000000
#define BASIC_STEPS_PER_REV				200

#define NUMERIC_ERROR					0.0001

#define CUTOUT_SLOWING_VELO_TAIL_FACTOR	0.8

typedef enum {STEPPER_FORWARD, STEPPER_BACKWARD}
stepperMotorDirection_t;

typedef struct{
	GPIO_TypeDef * enPort, *dirPort;
	uint16_t enPin, dirPin;

	TIM_HandleTypeDef * pulseTimerHandler;
	uint32_t pulseTimerChannel;

	TIM_HandleTypeDef * stateUpdateTimerHandler;
	uint32_t stateUpdateTimerChannel;

	logic_t enLogic, dirLogic;

	uint32_t stepCounter, settedPosition_Pulses, stepsWhenMotorHasToStartDeaccelerate;
	bool isPostionReached;
	bool isSlowing;
	double settedPosition, currentPosition;
	double settedVelocity, currentVelocity, prevVelocity;
	double settedAcceleration, currentAcceleration;

	double maxVelocity, maxAcceleration;

	double stepsPerMM;

	uint32_t mcuClock_Hz, microstepping;

} stepperMotor_t;

void stepperMotor_Init(stepperMotor_t*,
		GPIO_TypeDef *, GPIO_TypeDef *,
		uint32_t, uint32_t,
		TIM_HandleTypeDef *, uint32_t,
		TIM_HandleTypeDef *, uint32_t,
		logicType_t, logicType_t,
		uint32_t mic);

void stepperMotor_PulseFinishedCallback(stepperMotor_t*);
void stepperMotor_Update(stepperMotor_t*);
void stepperMotor_SetPosition(stepperMotor_t*, double, bool);
void stepperMotor_SetVelocity_rps(stepperMotor_t*, double);
void stepperMotor_SetAcceleration(stepperMotor_t*, double);
void stepperMotor_Start(stepperMotor_t *);
void stepperMotor_Stop(stepperMotor_t *);
void stepperMotor_UpdateTimerStart(stepperMotor_t *);
void stepperMotor_UpdateTimerStop(stepperMotor_t *);
void stepperMotor_SetRevoluteDirection(stepperMotor_t *, stepperMotorDirection_t);

void stepperMotor_DisableMotor(stepperMotor_t*);
void stepperMotor_EnableMotor(stepperMotor_t *);

double abs_d(double);
uint32_t stepperMotor_ConvVeloToFreq(stepperMotor_t *, double);
double stepperMotor_ConvFreqToVelo(stepperMotor_t *, uint32_t);

#endif /* INC_STEPPERMOTOR_H_ */
