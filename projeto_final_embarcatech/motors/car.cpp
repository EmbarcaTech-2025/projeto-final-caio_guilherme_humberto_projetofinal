#include<stdio.h>
#include "car.h"

car::car(uint8_t pin1_left, uint8_t pin2_left, 
         uint8_t pin1_right, uint8_t pin2_right, 
         uint8_t pin1_back, uint8_t pin2_back)

    : motor_left(pin1_left, pin2_left),       
      motor_right(pin1_right, pin2_right),     
      motor_back(pin1_back, pin2_back)     
{
   
}


void car::configureCar()
{
    this->motor_left.configureMotors();
    this->motor_right.configureMotors();
    this->motor_back.configureMotors();
}

// A assinatura da função agora aceita um controle com sinal (positivo ou negativo)
void car::moveForward(uint16_t speed, int16_t velAngularControl)
{
    // Usamos um tipo temporário maior (int32_t) para fazer os cálculos com segurança
    int32_t speed_left_temp;
    int32_t speed_right_temp;

    // Calcula a velocidade desejada para cada roda
    speed_left_temp = speed - velAngularControl;
    speed_right_temp = speed + velAngularControl;

    // --- Travas de Segurança (Clamping) ---
    // Garante que a velocidade final não seja negativa
    if (speed_left_temp < 0) {
        speed_left_temp = 0;
    }
    if (speed_right_temp < 0) {
        speed_right_temp = 0;
    }

    // Garante que a velocidade final não ultrapasse o valor máximo do PWM (ex: 4095)
    uint16_t max_pwm = 1020;
    if (speed_left_temp > max_pwm) {
        speed_left_temp = max_pwm;
    }
    if (speed_right_temp > max_pwm) {
        speed_right_temp = max_pwm;
    }

    printf("Speed Left Temp: %d | Speed Right Temp: %d\n", speed_left_temp, speed_right_temp);
    // Converte os valores seguros de volta para uint16_t e envia aos motores
    this->motor_left.setSpeedFoward((uint16_t)speed_left_temp);
    this->motor_right.setSpeedFoward((uint16_t)speed_right_temp);
    
    // O motor de trás continua com a velocidade base
    this->motor_back.setSpeedFoward(speed > max_pwm ? max_pwm : speed);
}

/**
 * @brief Move o carro para trás com uma velocidade base e aplica um controle de direção.
 * @param speed A velocidade base para o movimento de ré (valor positivo).
 * @param velAngularControl O sinal de correção do controlador PI (pode ser positivo ou negativo).
 */
void car::moveBackward(uint16_t speed, int16_t velAngularControl)
{
    // Usamos um tipo temporário maior (int32_t) para fazer os cálculos com segurança
    int32_t speed_left_temp;
    int32_t speed_right_temp;

    // --- LÓGICA DE DIREÇÃO INVERTIDA NA RÉ ---
    // Para virar à direita (velAngularControl > 0) ao dar ré, a roda
    // esquerda precisa girar mais rápido (para trás) e a direita mais devagar.
    speed_left_temp = speed + velAngularControl;
    speed_right_temp = speed - velAngularControl;

    // --- Travas de Segurança (Clamping) ---
    // Garante que a velocidade final não seja negativa
    if (speed_left_temp < 0) {
        speed_left_temp = 0;
    }
    if (speed_right_temp < 0) {
        speed_right_temp = 0;
    }

    // Garante que a velocidade final não ultrapasse o valor máximo do PWM (ex: 4095)
    uint16_t max_pwm = 1020;
    if (speed_left_temp > max_pwm) {
        speed_left_temp = max_pwm;
    }
    if (speed_right_temp > max_pwm) {
        speed_right_temp = max_pwm;
    }

    printf("Speed Left Temp (Back): %d | Speed Right Temp (Back): %d\n", speed_left_temp, speed_right_temp);
    // --- ACIONAMENTO DOS MOTORES PARA TRÁS ---
    // Converte os valores seguros de volta para uint16_t e chama o método de ré
    this->motor_left.setSpeedBackward((uint16_t)speed_left_temp);
    this->motor_right.setSpeedBackward((uint16_t)speed_right_temp);
    
    // O motor de trás continua com a velocidade base de ré
    this->motor_back.setSpeedBackward(speed > max_pwm ? max_pwm : speed);
}


/**
 * @brief Gira o carro para a direita no próprio eixo (sentido horário).
 * @param speed A velocidade de rotação (0 a 4095).
 */
void car::moveRight(uint16_t speed, uint8_t direction)
{
    uint16_t max_pwm = 1020;

    // Garante que a velocidade não ultrapasse o valor máximo
    if (speed > max_pwm) {
        speed = max_pwm;
    }

    // Para girar para a direita, a roda esquerda vai para frente
    this->motor_left.setSpeedFoward(speed);
    // e a roda direita vai para 
    this->motor_right.stop();
    
    if(direction == 1){
        this->motor_back.setSpeedFoward(800);
        return;
    }

    this->motor_back.setSpeedBackward(800);
}

/**
 * @brief Gira o carro para a esquerda no próprio eixo (sentido anti-horário).
 * @param speed A velocidade de rotação (0 a 4095).
 */
void car::moveLeft(uint16_t speed,uint8_t direction)
{
    uint16_t max_pwm = 1020;

    // Garante que a velocidade não ultrapasse o valor máximo
    if (speed > max_pwm) {
        speed = max_pwm;
    }

    // Para girar para a esquerda, a roda esquerda vai para trás
    this->motor_left.stop();
    // e a roda direita vai para frente.
    this->motor_right.setSpeedFoward(speed);

    // O motor de trás fica parado para permitir o giro.

    if(direction == 1){
        this->motor_back.setSpeedFoward(1000);
        return;
    }

    this->motor_back.setSpeedBackward(1000);
    
}

// Em car.cpp

// ... (implementação dos outros métodos como moveLeft, etc.) ...


/**
 * @brief Para todos os motores do carro.
 */
void car::stop()
{
    this->motor_left.stop();
    this->motor_right.stop();
    this->motor_back.stop();
}
