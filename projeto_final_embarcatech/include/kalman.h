// kalman.h
#ifndef KALMAN_H
#define KALMAN_H

class KalmanFilter1D {
private:
    double Q; // Ruído do processo
    double R; // Ruído da medição
    double P; // Incerteza da estimativa
    double x; // O estado estimado (o valor filtrado)

public:
    /**
     * @brief Construtor do Filtro de Kalman 1D.
     * @param q Ruído do processo. Quão rápido você espera que a velocidade mude.
     * @param r Ruído da medição. Quão "ruidoso" é o seu sensor.
     * @param p Incerteza inicial da estimativa.
     * @param initial_value Valor inicial para o estado.
     */
    KalmanFilter1D(double q, double r, double p, double initial_value);

    /**
     * @brief Executa um ciclo do filtro.
     * @param measurement A nova medição ruidosa do sensor.
     * @param deltaTime O tempo passado desde a última atualização.
     * @return O valor filtrado.
     */
    double update(double measurement, double deltaTime);
};

#endif // KALMAN_H