#ifndef CAR_H
#define CAR_H

#include "motors.h"
#include "pwm.h"

class car
{
    private:
        motors motor_left;
        motors motor_right;
        motors motor_back;
    public:

    car(uint8_t pin1_left, uint8_t pin2_left, uint8_t pin1_right, uint8_t pin2_right, uint8_t pin1_back, uint8_t pin2_back);
    void configureCar();
    void moveForward(uint16_t speed, int16_t velAngularControl);
    void moveBackward(uint16_t speed, int16_t velAngularControl);
    void moveLeft(uint16_t speed);
    void moveRight(uint16_t speed);
    void stop(); 
};

#endif