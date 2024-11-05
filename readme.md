# Motion Control Library
### About
Mini-library for handling basic actuators and sensors from the microcontroller level, designed to accelerate prototyping of machines/devices where the microcontroller is the control unit.

The library simplifies:
- ON/OFF control of DC motors
- position and rotational speed control of stepper motors using STEP/DIR drivers
- handling of ON/OFF sensors (limit switches)
- UART communication
- creating an abstraction layer for logic levels, allowing the definition of which logic level (HIGH/LOW) represents ACTIVE/INACTIVE, facilitating work with components controlled by high or low states.

### Usage
The library is portable across different STM controllers. To use the library, copy the `Motion_control_library` folder into your project in CubeIDE (in CubeIDE, add the additional source/header files to your project).