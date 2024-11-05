#ifndef MACHINESTATES_H
#define MACHINESTATES_H

typedef enum {
	MACHINE_WORKING,
	MACHINE_ERROR,
	MACHINE_TIMEOUT,
	MACHINE_HOME
} machineState_t;

typedef enum {
	MOTOR_WORKING,
	MOTOR_OFF,
	MOTOR_ERROR
} motorState_t;

#endif
