#pragma once
#include <stdint.h>

void leds_init(void);
void leds_tick_1ms(void);
void led_rx_pulse(void);
void led_tx_pulse(void);
void leds_set_bus_active(uint8_t active);
