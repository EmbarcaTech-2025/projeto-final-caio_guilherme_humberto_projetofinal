#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdint>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"

#include "car.h"
#include "mqtt_control.h"

// --------- Tópicos ---------
static const char* TOPIC_CMD     = "escola/sala1/carro/cmd";    // UP/DOWN/LEFT/RIGHT/STOP / SPEED:NNNN
static const char* TOPIC_SPEED   = "escola/sala1/carro/speed";  // 0..4095
static const char* TOPIC_STATUS  = "escola/sala1/carro/status"; // feedback
static const char* TOPIC_TEMP    = "escola/sala1/temperatura";  // °C

// --------- Estado ---------
static mqtt_client_t* g_mqtt = nullptr;
static ip_addr_t g_broker;
static repeating_timer_t g_pub_timer;
static car* g_car = nullptr;
static uint16_t g_speed = 3000;

// giro/curvatura por setas
static const int16_t TURN_ANG = 1200;

static char g_client_id[32];
static char g_user[32];
static char g_pass[64];

// buffers para mensagem recebida
static char   g_last_topic[128];
static size_t g_rx_len = 0;
static char   g_rx_buf[256];

// --------- Utilidades ---------
static inline void str_toupper(char* s) { for (; *s; ++s) *s = (char)toupper((unsigned char)*s); }
static inline void str_trim(char* s) {
    char* p = s; while (*p && isspace((unsigned char)*p)) ++p;
    if (p != s) memmove(s, p, strlen(p)+1);
    size_t n = strlen(s); while (n && isspace((unsigned char)s[n-1])) s[--n] = 0;
}

// Temp interna RP2040 (~aprox)
static float read_internal_temp_c() {
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    uint16_t raw = adc_read();
    const float VREF = 3.3f;
    float volt = raw * VREF / 4095.0f;
    return 27.0f - ((volt - 0.706f) / 0.001721f);
}

static void publish_text(const char* topic, const char* msg, u8_t qos=0, u8_t retain=0) {
    // lwIP 2.1.x: precisa callback e arg (mesmo que NULL)
    err_t err = mqtt_publish(g_mqtt, topic, msg, (u16_t)strlen(msg), qos, retain, /*cb*/NULL, /*arg*/NULL);
    if (err != ERR_OK) printf("mqtt_publish(%s) falhou: %d\n", topic, (int)err);
}

static void publish_temperature() {
    char buf[32]; snprintf(buf, sizeof(buf), "%.2f", (double)read_internal_temp_c());
    publish_text(TOPIC_TEMP, buf, 0, 0);
}

// ---------- Callbacks MQTT ----------
static void on_incoming_publish(void* /*arg*/, const char* topic, u32_t /*tot_len*/) {
    // Este callback vem primeiro (tópico e tamanho total)
    strncpy(g_last_topic, topic, sizeof(g_last_topic)-1);
    g_last_topic[sizeof(g_last_topic)-1] = 0;
    g_rx_len = 0;
}

static void on_incoming_data(void* /*arg*/, const u8_t* data, u16_t len, u8_t flags) {
    if (g_rx_len + len >= sizeof(g_rx_buf)) { g_rx_len = 0; return; }
    memcpy(g_rx_buf + g_rx_len, data, len);
    g_rx_len += len;

    if (flags & MQTT_DATA_FLAG_LAST) {
        g_rx_buf[g_rx_len] = 0;
        char payload[256]; strncpy(payload, g_rx_buf, sizeof(payload)-1); payload[sizeof(payload)-1] = 0;
        str_trim(payload);

        if (strcmp(g_last_topic, TOPIC_CMD) == 0) {
            char up[256]; strncpy(up, payload, sizeof(up)-1); up[sizeof(up)-1]=0; str_toupper(up);

            // Comandos aceitos (em maiúsculas): UP/DOWN/LEFT/RIGHT/STOP/PARAR (+ aliases)
            if (!strcmp(up, "UP") || !strcmp(up, "F") || !strcmp(up, "FORWARD") || !strcmp(up, "↑")) {
                g_car->moveForward(g_speed, 0);
                sleep_ms(3000); 
                g_car->stop();
                publish_text(TOPIC_STATUS, "UP");
            } else if (!strcmp(up, "DOWN") || !strcmp(up, "B") ||  !strcmp(up, "BACK") || !strcmp(up, "BACKWARD") || !strcmp(up, "↓")) {
                g_car->moveBackward(g_speed, 0);
                g_car->stop();
                sleep_ms(3000);
                publish_text(TOPIC_STATUS, "DOWN");
            } else if (!strcmp(up, "LEFT") || !strcmp(up, "L") || !strcmp(up, "←")) {
                g_car->moveLeft(g_speed, 1);
                sleep_ms(3000);
                g_car->stop();
                sleep_ms(3000);
                publish_text(TOPIC_STATUS, "LEFT");
            } else if (!strcmp(up, "RIGHT") || !strcmp(up, "R") || !strcmp(up, "→")) {
                g_car->moveRight(g_speed, 1);
                sleep_ms(3000);
                g_car->stop();
                publish_text(TOPIC_STATUS, "RIGHT");
            } else if (!strcmp(up, "STOP") || !strcmp(up, "PARAR") || !strcmp(up, "S") || !strcmp(up, "SPACE")) {
                g_car->stop();
                publish_text(TOPIC_STATUS, "stop");
            } else {
                publish_text(TOPIC_STATUS, "unknown-cmd");
            }
        } else if (strcmp(g_last_topic, TOPIC_SPEED) == 0) {
            int v = atoi(payload);
            if (v < 0) v = 0; if (v > 4095) v = 4095;
            g_speed = (uint16_t)v;
            char msg[32]; snprintf(msg, sizeof(msg), "speed=%u", g_speed);
            publish_text(TOPIC_STATUS, msg);
        }

        g_rx_len = 0;
    }
}

static void do_subscribes() {
    auto sub = [&](const char* topic){
        err_t err = mqtt_subscribe(g_mqtt, topic, 0, NULL, NULL);
        printf("SUB %s -> %d\n", topic, (int)err);
    };
    sub(TOPIC_CMD);
    sub(TOPIC_SPEED);
}

static void mqtt_conn_cb(mqtt_client_t *client, void */*arg*/, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT conectado!\n");
        // Ordem correta: publish_cb, data_cb
        mqtt_set_inpub_callback(client, on_incoming_publish, on_incoming_data, NULL);
        do_subscribes();
        publish_text(TOPIC_STATUS, "online");
    } else {
        printf("MQTT desconectado (status=%d).\n", (int)status);
    }
}

// ---------- API ----------
void mqtt_begin(car* car_ptr,
                const char* ssid,
                const char* pass,
                const char* broker_ip,
                uint16_t broker_port,
                const char* mqtt_user,
                const char* mqtt_pass) {
    g_car = car_ptr;
    strncpy(g_user, mqtt_user ? mqtt_user : "", sizeof(g_user)-1);
    strncpy(g_pass, mqtt_pass ? mqtt_pass : "", sizeof(g_pass)-1);
    snprintf(g_client_id, sizeof(g_client_id), "pico-%u", (unsigned)to_ms_since_boot(get_absolute_time()) & 0xFFFF);

    // Wi-Fi
    if (cyw43_arch_init()) { printf("cyw43_arch_init FAIL\n"); return; }
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi %s ...\n", ssid);
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Wi-Fi connect FAIL\n");
        return;
    }
    printf("Wi-Fi OK.\n");
    #include "lwip/netif.h"
    #include "lwip/ip4_addr.h"
    
    extern struct netif* netif_default;
    printf("Pico IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
    printf("Broker IP: %s\n", ipaddr_ntoa(&g_broker));
    // MQTT
    ipaddr_aton(broker_ip, &g_broker);
    g_mqtt = mqtt_client_new();

    // Ordem correta: publish_cb, data_cb
    mqtt_set_inpub_callback(g_mqtt, on_incoming_publish, on_incoming_data, NULL);

    struct mqtt_connect_client_info_t ci {};
    ci.client_id   = g_client_id;
    ci.client_user = g_user[0] ? g_user : NULL;
    ci.client_pass = g_pass[0] ? g_pass : NULL;
    ci.keep_alive  = 60;

    printf("Conectando MQTT em %s:%u ...\n", broker_ip, broker_port);
    // NAS VERSÕES ATUAIS: mqtt_client_connect(...)
    err_t err = mqtt_client_connect(g_mqtt, &g_broker, broker_port, mqtt_conn_cb, NULL, &ci);
    if (err != ERR_OK) printf("mqtt_client_connect falhou: %d\n", (int)err);

    // ADC p/ temperatura
    adc_init();
    adc_set_temp_sensor_enabled(true);
}

static bool timer_cb(repeating_timer_t*) {
    if (g_mqtt) publish_temperature();
    return true; // manter
}

void mqtt_start_periodic_publish_temp_ms(uint32_t every_ms) {
    add_repeating_timer_ms((int64_t)every_ms, timer_cb, NULL, &g_pub_timer);
}