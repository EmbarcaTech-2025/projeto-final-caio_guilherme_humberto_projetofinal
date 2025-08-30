#pragma once
#include <stdint.h>
class car;
// Inicializa Wi‑Fi e MQTT; conecta ao broker e assina os tópicos de controle.
void mqtt_begin(car* car_ptr,
                const char* ssid,
                const char* pass,
                const char* broker_ip,
                uint16_t    broker_port,
                const char* mqtt_user,
                const char* mqtt_pass);
// Publica temperatura interna periodicamente (em ms).
void mqtt_start_periodic_publish_temp_ms(uint32_t every_ms);