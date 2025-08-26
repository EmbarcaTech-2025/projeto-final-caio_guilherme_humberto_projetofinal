#ifndef MOTORS_H
#define MOTORS_H

#include "pwm.h"

class motors
{
private:
    uint8_t pins[2];

public:

    motors(uint8_t pin1, uint8_t pin2);

    void configureMotors();
    void setSpeedFoward(uint16_t speed);
    void setSpeedBackward(uint16_t speed);
    void stop();
    void moveRight(uint16_t speed);
    void moveLeft(uint16_t speed);
};



#endif