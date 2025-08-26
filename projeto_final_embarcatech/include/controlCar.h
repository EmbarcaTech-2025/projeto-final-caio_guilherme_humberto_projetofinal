#ifndef CONTROLCAR_H
#define CONTROLCAR_H

#include "mpu6050_i2c.h"

void configureMPU6050();

int control(double velAngularReference , double velAngularMedicao, double deltaTime);
double velAngulzarZ();

#endif