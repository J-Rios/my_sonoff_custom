#include "sonoff_standard_hw.h"

/**************************************************************************************************/

/* Globals */

SonoffDev Sonoff;

/**************************************************************************************************/

void setup()
{
    // Initialize Serial
    Serial.begin(115200);
    Serial.println("System Started.");

    // Initialize Sonoff hardware
    Sonoff.rele_init();
    Sonoff.led_init();
    Sonoff.button_init();

    Serial.println("Hardware initialized.");
    Serial.println("");
}

void loop()
{
    static bool btn_pressed = false;

    if(Sonoff.button_pressed())
    {
        if(!btn_pressed)
        {
            Serial.println("Button pressed.");
            Sonoff.led_on();
            Sonoff.rele_on();
            btn_pressed = true;
        }
    }
    else
    {
        if(btn_pressed)
        {
            Serial.println("Button released.");
            Sonoff.led_off();
            Sonoff.rele_off();
            btn_pressed = false;
        }
    }
    delay(100);
}
