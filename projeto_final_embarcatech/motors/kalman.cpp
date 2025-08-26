// kalman.cpp
#include "kalman.h"

KalmanFilter1D::KalmanFilter1D(double q, double r, double p, double initial_value) {
    this->Q = q;
    this->R = r;
    this->P = p;
    this->x = initial_value;
}

double KalmanFilter1D::update(double measurement, double deltaTime) {
    // --- Etapa de PREVISÃO ---
    // A previsão do estado é o estado anterior (modelo simples)
    // A incerteza da previsão aumenta com o tempo e o ruído do processo
    this->P = this->P + this->Q * deltaTime;

    // --- Etapa de CORREÇÃO ---
    // Calcula o Ganho de Kalman
    double K = this->P / (this->P + this->R);

    // Atualiza a estimativa do estado com a nova medição
    this->x = this->x + K * (measurement - this->x);

    // Atualiza a incerteza da estimativa
    this->P = (1 - K) * this->P;

    return this->x;
}