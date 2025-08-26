#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

void configurePwm (uint8_t pin);
void setPwm(uint8_t pin, uint16_t value);

#endif