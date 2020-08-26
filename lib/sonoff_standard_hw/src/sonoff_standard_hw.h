/* Include Guard */

#ifndef SONOFFDEV_STANDARD_HW_H_
#define SONOFFDEV_STANDARD_HW_H_

/**************************************************************************************************/

/* Defines & Macros */

// Input & Output GPIOs (Sonoff basic R2 V1.0 2017-10-11)
#define P_I_BTN 0
#define P_O_RELE 12
#define P_O_LED 13
#define P_I_RF_RX 14

/**************************************************************************************************/

/* Libraries */

#include <Arduino.h>

/**************************************************************************************************/

class SonoffDev
{
    public:
        // Constuctor
        SonoffDev(void);

        // Button methods
        void button_init(void);
        bool button_is_pressed(void);
        bool button_is_released(void);
        bool button_was_pressed(void);

        // LED methods
        void led_init(void);
        void led_on(void);
        void led_off(void);
        void led_toggle(void);
        bool led_is_on(void);

        // Rele methods
        void relay_init(void);
        void relay_on(void);
        void relay_off(void);
        void relay_toggle(void);
        bool relay_is_on(void);

    private:
        int8_t btn_status;
        int8_t led_status;
        int8_t relay_status;
};

/**************************************************************************************************/

#endif