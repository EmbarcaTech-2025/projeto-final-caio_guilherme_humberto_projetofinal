#include <stdio.h>
#include "mpu6050_i2c.h"

static double kp = 0.6;
static double ki = 0.8;
static double kd = 0;

static int16_t accel_raw[3], gyro_raw[3], temp;
static double accel[3], gyro[3];

int control(double velAngularReference, double velAngularMedicao, double deltaTime)
{
    // 1. Calcula o erro atual
    double error = velAngularReference - velAngularMedicao;

    // 2. Acumula o erro (a parte Integral)
    //    A variável precisa ser 'static' para manter seu valor entre as chamadas da função.
    static double integral_error = 0;
    integral_error += error * deltaTime; // Multiplica pelo tempo para uma integral correta

    // 3. Calcula o sinal de controle (Proporcional + Integral)
    double controlSignal_double = (kp * error) + (ki * integral_error);
    
    // Converte para int antes de retornar
    return (int)controlSignal_double;
}


void configureMPU6050()
{
    mpu6050_setup_i2c();
    mpu6050_reset();
    sleep_ms(1000); // Give time to open Serial Monitor
    mpu6050_set_accel_range(0); // ±2g
}

double velAngulzarZ()
{
    mpu6050_read_raw(accel_raw, gyro_raw, &temp);
    gyro[2] = (double)gyro_raw[2] / 131.0f;
    return gyro[2];
}