#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pwm.h"

#define PERIOD 1024
#define DIVISOR 81

void configurePwm (uint8_t pin) // Configura o pino PWM com uma frequencia de 1500hz e um periodo de 4095
{
    uint slice;
    gpio_set_function(pin,GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice,DIVISOR);
    pwm_set_wrap(slice,PERIOD);
    pwm_set_gpio_level(pin,0);
    pwm_set_enabled(slice,true);
}

void setPwm(uint8_t pin, uint16_t value) // Coontrola o ciclo de trabalho do PWM
{
    pwm_set_gpio_level(pin,value);
}
