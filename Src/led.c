#include "stm32f0xx_hal.h"
#include "boards.h"
#include "led.h"

#define LED_PULSE_MS   50
#define FAST_BLINK_MS  100
#define SLOW_BLINK_MS  500

static volatile uint16_t rx_timer = 0;
static volatile uint16_t tx_timer = 0;
static uint8_t bus_active = 0;
static uint8_t boff_state = 0;
static uint16_t boff_counter = 0;
static uint8_t hb_state = 0;
static uint16_t hb_counter = 0;

void leds_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = IOPIN_LED_RX | IOPIN_LED_TX;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IOPIN_PORT, &GPIO_InitStruct);
}

void leds_set_bus_active(uint8_t active)
{
    bus_active = active;
}

void led_rx_pulse(void)
{
    rx_timer = LED_PULSE_MS;
}

void led_tx_pulse(void)
{
    tx_timer = LED_PULSE_MS;
}

void leds_tick_1ms(void)
{
    uint8_t rx_on = 0;
    uint8_t tx_on = 0;

    if (CAN->ESR & CAN_ESR_BOFF)
    {
        if (++boff_counter >= FAST_BLINK_MS)
        {
            boff_counter = 0;
            boff_state ^= 1;
        }
        rx_on = tx_on = boff_state;
        rx_timer = tx_timer = 0;
    }
    else if (!bus_active)
    {
        if (++hb_counter >= SLOW_BLINK_MS)
        {
            hb_counter = 0;
            hb_state ^= 1;
        }
        tx_on = hb_state;
        rx_timer = tx_timer = 0;
    }
    else
    {
        if (rx_timer)
        {
            rx_timer--;
            rx_on = 1;
        }
        if (tx_timer)
        {
            tx_timer--;
            tx_on = 1;
        }
    }

    HAL_GPIO_WritePin(IOPIN_PORT, IOPIN_LED_RX, rx_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(IOPIN_PORT, IOPIN_LED_TX, tx_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
