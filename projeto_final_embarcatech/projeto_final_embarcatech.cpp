#include <stdio.h>
#include "pico/stdlib.h"
#include "mqtt_control.h"
#include "car.h"

// ====== AJUSTE AQUI ======
// Wi-Fi (o que você me passou)
static const char* WIFI_SSID = "CAIOECAMYLA";
static const char* WIFI_PASS = "cv102002";

// IP do broker Mosquitto na rede (IP do PC/rasp que roda o mosquitto)
static const char* BROKER_IP     = "192.168.15.9";  // <— troque pelo IP certo
static const uint16_t BROKER_PORT = 1883;
static const char* MQTT_USER = "aluno";
static const char* MQTT_PASS = "caiovitor12";
// ==========================

// Pinos: (Esquerda: 16,17) (Direita: 18,19) (Traseiro/Centro: 8,9)
car myCar(16, 17, 18, 19, 8, 9);

int main() {
    stdio_init_all();
    sleep_ms(1500);
    printf("== Carro MQTT - Iniciando ==\n");

    // Configura PWM dos motores
    myCar.configureCar();
    printf("Motores configurados.\n");

    // Conecta Wi-Fi + MQTT e assina os tópicos de controle
    mqtt_begin(&myCar, WIFI_SSID, WIFI_PASS, BROKER_IP, BROKER_PORT, MQTT_USER, MQTT_PASS);

    // (Opcional) publicar temperatura interna a cada 5 s
    mqtt_start_periodic_publish_temp_ms(5000);

    // Loop ocioso – a pilha de rede roda em background com a lib threadsafe
    while (true) {
        tight_loop_contents();
        sleep_ms(10);
    }
}
