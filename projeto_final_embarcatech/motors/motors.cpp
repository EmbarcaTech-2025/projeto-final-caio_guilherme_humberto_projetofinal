#include "motors.h" // 1. Inclui a declaração da classe (o "cardápio")
#include <stdio.h> // Se você realmente precisar de printf aqui

// 2. Agora, definimos cada função prometida no .h usando "motors::"

// Definição do construtor
motors::motors(uint8_t pin1, uint8_t pin2)
{
    this->pins[0] = pin1;
    this->pins[1] = pin2;
}

// Definição do método configureMotors
void motors::configureMotors() // Configura os pinos PWM dos motores
{
    for(uint8_t i = 0; i < 2; i++)
    {
        // A função configurePwm() vem de "pwm.h", que foi incluído em "motors.h"
        configurePwm(this->pins[i]);
    }
}
 
// Definição do método setSpeedFoward
void motors::setSpeedFoward(uint16_t speed) // Controla a velocidade para ir para frente
{
    setPwm(this->pins[0], speed);
    setPwm(this->pins[1], 0);
}

// Definição do método setSpeedBackward
void motors::setSpeedBackward(uint16_t speed) // Controla a velocidade para ir para trás
{
    setPwm(this->pins[0], 0);
    setPwm(this->pins[1], speed);
}

void motors::stop()
{
    // Para o motor zerando o PWM em ambas as entradas
    setPwm(this->pins[0], 0);
    setPwm(this->pins[1], 0);
}

