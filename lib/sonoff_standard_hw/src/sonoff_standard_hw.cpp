/* Libraries */

#include "sonoff_standard_hw.h"

/**************************************************************************************************/

/* Sonoff Button ISR */

#if defined(SONOFF_BTN_ISR)

    const uint8_t SOFT_BTN_DEBOUNCE_DELAY = 100;

    volatile uint8_t _btn_isr_fire = 0;
    volatile int8_t _btn_status = HIGH;

    ICACHE_RAM_ATTR void isr_button(void)
    {
        if(_btn_isr_fire == 0)
        {
            _btn_isr_fire = 1;
            _btn_status = LOW;
        }
    }

#endif

/**************************************************************************************************/

/* Sonoff Constructor */

SonoffDev::SonoffDev(void)
{
    // Still uninitialized (-1)
    btn_status = -1;
    led_status = -1;
    relay_status = -1;
}

/**************************************************************************************************/

/* Sonoff Button Methods */

void SonoffDev::button_init(void)
{
    pinMode(P_I_BTN, INPUT_PULLUP);
    btn_status = digitalRead(P_I_BTN);
    #if defined(SONOFF_BTN_ISR)
        _btn_status = btn_status;
        attachInterrupt(digitalPinToInterrupt(P_I_BTN), isr_button, FALLING);
    #endif
}

bool SonoffDev::button_is_pressed(void)
{
    #if defined(SONOFF_BTN_ISR)
        if(_btn_isr_fire == 1)
        {
            delay(SOFT_BTN_DEBOUNCE_DELAY);
            btn_status = _btn_status;
            _btn_isr_fire = 0;
        }
        else
        {
            _btn_status = HIGH;
            btn_status = _btn_status;
        }
    #else
        btn_status = digitalRead(P_I_BTN);
    #endif

    return button_was_pressed();
}

bool SonoffDev::button_is_released(void)
{
    return !(button_is_pressed());
}

bool SonoffDev::button_was_pressed(void)
{
    if(btn_status == LOW)
        return true;
    else
        return false;
}

/**************************************************************************************************/

/* Sonoff LED Methods */

void SonoffDev::led_init(void)
{
    led_off();
    pinMode(P_O_LED, OUTPUT);
}

void SonoffDev::led_on(void)
{
    led_status = 1;
    digitalWrite(P_O_LED, LOW);
}

void SonoffDev::led_off(void)
{
    led_status = 0;
    digitalWrite(P_O_LED, HIGH);
}

void SonoffDev::led_toggle(void)
{
    if(led_status == 0)
        led_on();
    else
        led_off();
}

bool SonoffDev::led_is_on(void)
{
    if(led_status == 1)
        return true;
    else
        return false;
}

/**************************************************************************************************/

/* Sonoff Relay Methods */

void SonoffDev::relay_init(void)
{
    relay_off();
    pinMode(P_O_RELE, OUTPUT);
}

void SonoffDev::relay_on(void)
{
    relay_status = 1;
    digitalWrite(P_O_RELE, LOW);
}

void SonoffDev::relay_off(void)
{
    relay_status = 0;
    digitalWrite(P_O_RELE, HIGH);
}

void SonoffDev::relay_toggle(void)
{
    if(relay_status == 0)
        relay_on();
    else
        relay_off();
}

bool SonoffDev::relay_is_on(void)
{
    if(relay_status == 1)
        return true;
    else
        return false;
}
