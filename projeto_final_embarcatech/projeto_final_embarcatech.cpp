#include <stdio.h>
#include "pico/stdlib.h"

// Incluímos apenas o necessário para o movimento básico
#include "car.h"

// --- OBJETOS GLOBAIS ---

// Cria o objeto Carro com os pinos para cada motor
// (Esquerda, Direita, Trás/Centro)
car myCar(16, 17, 18, 19, 8, 9);


int main()
{
    // --- BLOCO DE INICIALIZAÇÃO ---
    stdio_init_all();
    // Um tempo generoso para você abrir o monitor serial
    sleep_ms(5000); 

    printf("--- INICIANDO TESTE DE MOVIMENTO SEM PID ---\n\n");

    // Configura apenas os motores. Não precisamos do sensor agora.
    myCar.configureCar();
    printf("Motores configurados.\n\n");

    // --- PARÂMETROS DO TESTE ---
    uint16_t test_speed = 3000;       // Velocidade base para os testes (0-4095)
    uint32_t move_duration_ms = 3000; // Duração de cada movimento (3 segundos)
    uint32_t stop_duration_ms = 1500; // Duração da pausa entre os movimentos (1.5 segundos)


    // --- SEQUÊNCIA DE TESTES ---

    // 1. Teste: Mover para Frente
    printf("--> Teste 1: Movendo para FRENTE por %lu ms...\n", move_duration_ms);
    // Note que o segundo parâmetro (controle angular) é sempre 0
    myCar.moveForward(test_speed, 0);
    sleep_ms(move_duration_ms);
    myCar.stop();
    printf("   Parado.\n");
    sleep_ms(stop_duration_ms);

    // 2. Teste: Mover para Trás
    printf("--> Teste 2: Movendo para TRAS por %lu ms...\n", move_duration_ms);
    // O controle angular também é 0 aqui
    myCar.moveBackward(test_speed, 0);
    sleep_ms(move_duration_ms);
    myCar.stop();
    printf("   Parado.\n");
    sleep_ms(stop_duration_ms);

    // 3. Teste: Girar para a Direita
    printf("--> Teste 3: Girando para a DIREITA por %lu ms...\n", move_duration_ms / 2); // Metade do tempo para não girar muito
    myCar.moveRight(test_speed);
    sleep_ms(move_duration_ms / 2);
    myCar.stop();
    printf("   Parado.\n");
    sleep_ms(stop_duration_ms);

    // 4. Teste: Girar para a Esquerda
    printf("--> Teste 4: Girando para a ESQUERDA por %lu ms...\n", move_duration_ms / 2);
    myCar.moveLeft(test_speed);
    sleep_ms(move_duration_ms / 2);
    myCar.stop();
    printf("   Parado.\n");
    sleep_ms(stop_duration_ms);

    printf("\n--- SEQUENCIA DE TESTE CONCLUIDA ---\n");

    // Loop infinito para manter o programa rodando
    while (true) {
        tight_loop_contents();
    }
}