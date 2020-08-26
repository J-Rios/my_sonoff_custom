
/* Libraries */

// Standard C/C++ libraries
#include <string.h>

// Device libraries (Arduino ESP32/ESP8266 Cores)
#include <Arduino.h>
#ifdef ESP8266
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <EEPROM.h>

// Custom libraries
#include <sonoff_standard_hw.h>
#include <utlgbotlib.h>
#include <NTPClient.h>

// Builtin headers
#include "config.h"
#include "text.h"
#include "utils.h"
#include "nvm.h"
#include "timecfg.h"
//#include "ota.h"

/*************************************************************************************************/

/* Functions Prototypes */

void control_by_button(void);
void control_time_programmed(void);
void control_by_telegram(void);

void wifi_init_stat(void);
bool wifi_handle_connection(void);

void nvm_load(void);

/*************************************************************************************************/

/* Globals */

// Create Sonoff object
SonoffDev Sonoff;

// Create Telegram Bot object
uTLGBot TlgBot(TLG_TOKEN);

// Define NTP Client to get time
WiFiUDP WifiUDP;
NTPClient NTPclient(WifiUDP, NTP_SERVER_ADDR, 0, NTP_UPDATE_EACH_MIN);

// Configured automatically turn on and off times
CTimeCfg TimeCfg;

// First time system run
uint8_t first_time_boot = 1;

// Timezone
int8_t time_zone = 0;

// NTP enable
uint8_t ntp_enable = 1;

/*************************************************************************************************/

void setup()
{
    // Initialize Serial
    Serial.begin(115200);
    delay(5000);
    Serial.println("System Started.");

    // Initialize Sonoff hardware
    Sonoff.relay_init();
    Sonoff.led_init();
    Sonoff.button_init();
    Serial.println("Hardware initialized.");

    // Initialize NVM and restore stored data
    Serial.println("Initializing NVM...");
    EEPROM.begin(NVM_SIZE_TO_USE);
    nvm_load();
    Serial.println("NVM initialized and stored data restored.");

    // Initialize WiFi station connection
    wifi_init_stat();

    // Enable Bot debug
    TlgBot.set_debug(DEBUG_LEVEL_UTLGBOT);

    Serial.println("");
}

void loop()
{
    //ota_loop();

    control_by_button();
    control_time_programmed();

    // Check if WiFi is connected
    if(wifi_handle_connection())
        control_by_telegram();

    delay(100);
}

/*************************************************************************************************/

/* Functions */

// Check and control by button
void control_by_button(void)
{
    if(Sonoff.button_was_pressed())
    {
        if(Sonoff.button_is_released())
        {
            Serial.println("Button released.");
            Sonoff.led_toggle();
            Sonoff.relay_toggle();
        }
    }
    else
    {
        if(Sonoff.button_is_pressed())
            Serial.println("Button pressed.");
    }
}

// Check and automatically control by time programmed on/off
void control_time_programmed(void)
{
    t_cfg_time time_cfg;
    int8_t turn_on_off = -1;
    uint8_t current_hour = NTPclient.getHours();
    uint8_t current_min = NTPclient.getMinutes();
    uint8_t current_sec = NTPclient.getSeconds();

    // Determine if we need to turn on or off the Relay from configured times
    for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
    {
        // Ignore if not configured
        if(TimeCfg.get(i, &time_cfg) == false)
            break;

        // Due times has been stored in order, we can check in this way
        if((current_hour >= time_cfg.t.hour) && (current_min >= time_cfg.t.min) &&
        (current_sec >= time_cfg.t.sec))
        {
            if(time_cfg.on_off == 0)
                turn_on_off = 0;
            else if(time_cfg.on_off == 1)
                turn_on_off = 1;
        }
    }

    // Turn on/off Sonoff LED and Relay
    if(turn_on_off == 0)
    {
        if(Sonoff.relay_is_on() == true)
        {
            Serial.printf("Time to turn off the Relay (%s).\n\n",
                    NTPclient.getFormattedTime().c_str());
            Sonoff.led_off();
            Sonoff.relay_off();
        }
    }
    else if (turn_on_off == 1)
    {
        if(Sonoff.relay_is_on() == false)
        {
            Serial.printf("Time to turn on the Relay (%s).\n\n",
                    NTPclient.getFormattedTime().c_str());
            Sonoff.led_on();
            Sonoff.relay_on();
        }
    }
}

// Check and control by Telegram
void control_by_telegram(void)
{
    static char str_to_send[MAX_TLG_MSG_SIZE];
    char str_time[MAX_TIME_SIZE];
    t_simple_time t;
    t_cfg_time time_cfg;
    int8_t rc = RC_TIMECFG_OK;

    // Check for Bot received messages
    if(TlgBot.getUpdates())
    {
        char* chat_id = TlgBot.received_msg.chat.id;
        char* msg_text = TlgBot.received_msg.text;

        // Show received message text
        Serial.println("");
        Serial.println("Received telegram message:");
        Serial.print("  Chat_ID: "); Serial.println(chat_id);
        Serial.print("  Text: "); Serial.println(msg_text);
        Serial.println("");

        // Ignore if the message doesn't come from allowed group
        if(strncmp(chat_id, TLG_GROUP_ID, strlen(TLG_GROUP_ID)) != 0)
            return;

        // If /start command was received
        if(strncmp(msg_text, "/start", strlen("/start")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /start received.");

            // Send a Telegram message for start
            //TlgBot.sendReplyKeyboardMarkup(chat_id, TEXT_START, keyboard);
            TlgBot.sendMessage(chat_id, TEXT_START);
        }

        // If /help command was received
        else if(strncmp(msg_text, "/help", strlen("/help")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /help received.");

            // Send a Telegram message for start
            TlgBot.sendMessage(chat_id, TEXT_HELP);
        }

        // If /on command was received
        else if(strncmp(msg_text, "/on", strlen("/on")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /on received.");

            // Turn on Sonoff LED and Relay
            Sonoff.led_on();
            Sonoff.relay_on();

            // Send a Telegram message to notify that the LED has been turned on
            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_ON);
            Serial.println(str_to_send);
            TlgBot.sendMessage(chat_id, str_to_send);
        }

        // If /off command was received
        else if(strncmp(msg_text, "/off", strlen("/off")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /off received.");

            // Turn off Sonoff LED and Relay
            Sonoff.led_off();
            Sonoff.relay_off();

            // Send a Telegram message to notify that the Relay has been turned off
            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_OFF);
            Serial.println(str_to_send);
            TlgBot.sendMessage(chat_id, str_to_send);
        }

        // If /status command was received
        else if(strncmp(msg_text, "/status", strlen("/status")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /status received.");

            // Send a Telegram message to notify actual LED status
            if(Sonoff.relay_is_on())
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_STATUS_ON);
            else
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_STATUS_OFF);
            Serial.println(str_to_send);
            TlgBot.sendMessage(chat_id, str_to_send);
        }

        // If /timeon command was received
        else if(strncmp(msg_text, "/timeon", strlen("/timeon")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /timeon received.");

            // Check for provided arguments
            if(cstr_count_words(msg_text, strlen(msg_text)) < 2)
            {
                // No arguments provided

                // Get configured turn on times
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TON_GET);
                for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
                {
                    // Ignore if not configured
                    if(TimeCfg.get(i, &time_cfg) == false)
                        break;

                    // Ignore if not turn on
                    if(time_cfg.on_off != 1)
                        continue;

                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            time_cfg.t.hour, time_cfg.t.min, time_cfg.t.sec);
                    strncat(str_to_send, str_time, MAX_TIME_SIZE);
                }

                // Send message
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else
            {
                // Arguments provided
                char* ptr_cmd = msg_text;
                char* ptr_argv;
                int8_t remove_add = -1;
                bool time_cfg_full = false;
                bool time_cfg_found = false;
                bool fail = false;

                // Deep into a new scope
                while(1)
                {
                    // Ignore if unexpected length in provided command
                    if((strlen(ptr_cmd) != strlen("/timeon set XX:YY:ZZ")) &&
                    (strlen(ptr_cmd) != strlen("/timeon unset XX:YY:ZZ")))
                    {
                        fail = true;
                        break;
                    }

                    // Point to second command argument
                    ptr_argv = strstr(ptr_cmd, " ");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Check for command operation (set/unset)
                    if(strncmp(ptr_argv, "unset", strlen("unset")) == 0)
                        remove_add = 0;
                    else if(strncmp(ptr_argv, "set", strlen("set")) == 0)
                        remove_add = 1;
                    else
                    {
                        fail = true;
                        break;
                    }

                    // Point to third command argument
                    ptr_argv = strstr(ptr_argv, " ");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get hour
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.hour), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid hour value
                    if(t.hour > 23)
                    {
                        fail = true;
                        break;
                    }

                    // Point to minutes
                    ptr_argv = strstr(ptr_argv, ":");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get minute
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.min), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid hour value
                    if(t.min > 59)
                    {
                        fail = true;
                        break;
                    }

                    // Point to seconds
                    ptr_argv = strstr(ptr_argv, ":");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get seconds
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.sec), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid second value
                    if(t.sec > 59)
                    {
                        fail = true;
                        break;
                    }

                    // Provided turn on time
                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            t.hour, t.min, t.sec);

                    // Remove a configured turn on time
                    if(remove_add == 0)
                    {
                        rc = TimeCfg.remove(t);
                        if(rc == RC_TIMECFG_INVALID_INPUT)
                            fail = true;
                        else if(rc == RC_TIMECFG_NOT_FOUND)
                            time_cfg_found = false;
                        else
                            time_cfg_found = true;
                    }

                    // Configure a new turn on time
                    else if(remove_add == 1)
                    {
                        rc = TimeCfg.add(t, 1);
                        if(rc == RC_TIMECFG_INVALID_INPUT)
                            fail = true;
                        else if(rc == RC_TIMECFG_FOUND)
                            time_cfg_found = true;
                        else if(rc == RC_TIMECFG_FULL)
                            time_cfg_full = true;
                    }

                    // Store it into persistent memory
                    uint8_t tcfg_num = TimeCfg.get_num_time_cfg();
                    t_cfg_time* tcfg = TimeCfg.get_time_cfg();
                    nvm_write(NVM_ADDR_NUM_T_CFG, &tcfg_num, 1);
                    nvm_write(NVM_ADDR_T_CFG, (uint8_t*)tcfg, sizeof(tcfg[0])*MAX_CFG_TIMES);

                    break;
                }

                // Command result fail | success
                if(fail)
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TON_INVALID_ARG);
                else
                {
                    // Time to be removed
                    if(remove_add == 0)
                    {
                        // Unset success message | Has not been found message
                        if(time_cfg_found)
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TON_UNSET, str_time);
                        else
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TCFG_NOTFOUND);
                    }

                    // Time to be configured
                    else if(remove_add == 1)
                    {
                        // Has been found message | config time full | Set success message
                        if(time_cfg_found)
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TCFG_ALREADY);
                        else if(time_cfg_full == true)
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TCFG_FULL);
                        else
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TON_SET, str_time);
                    }
                }

                // Send message
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
        }

        // If /timeoff command was received
        else if(strncmp(msg_text, "/timeoff", strlen("/timeoff")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /timeoff received.");

            // Check for provided arguments
            if(cstr_count_words(msg_text, strlen(msg_text)) < 2)
            {
                // No arguments provided

                // Get configured turn off times
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TOFF_GET);
                for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
                {
                    // Ignore if not configured
                    if(TimeCfg.get(i, &time_cfg) == false)
                        break;

                    // Ignore if not turn off
                    if(time_cfg.on_off != 0)
                        continue;

                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            time_cfg.t.hour, time_cfg.t.min, time_cfg.t.sec);
                    strncat(str_to_send, str_time, MAX_TIME_SIZE);
                }

                // Send message
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else
            {
                // Arguments provided
                char* ptr_cmd = msg_text;
                char* ptr_argv;
                int8_t remove_add = -1;
                bool time_cfg_full = false;
                bool time_cfg_found = false;
                bool fail = false;

                // Deep into a new scope
                while(1)
                {
                    // Ignore if unexpected length in provided command
                    if((strlen(ptr_cmd) != strlen("/timeoff set XX:YY:ZZ")) &&
                    (strlen(ptr_cmd) != strlen("/timeoff unset XX:YY:ZZ")))
                    {
                        fail = true;
                        break;
                    }

                    // Point to second command argument
                    ptr_argv = strstr(ptr_cmd, " ");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Check for command operation (set/unset)
                    if(strncmp(ptr_argv, "unset", strlen("unset")) == 0)
                        remove_add = 0;
                    else if(strncmp(ptr_argv, "set", strlen("set")) == 0)
                        remove_add = 1;
                    else
                    {
                        fail = true;
                        break;
                    }

                    // Point to third command argument
                    ptr_argv = strstr(ptr_argv, " ");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get hour
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.hour), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid hour value
                    if(t.hour > 23)
                    {
                        fail = true;
                        break;
                    }

                    // Point to minutes
                    ptr_argv = strstr(ptr_argv, ":");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get minute
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.min), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid hour value
                    if(t.min > 59)
                    {
                        fail = true;
                        break;
                    }

                    // Point to seconds
                    ptr_argv = strstr(ptr_argv, ":");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get seconds
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.sec), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid second value
                    if(t.sec > 59)
                    {
                        fail = true;
                        break;
                    }

                    // Provided turn off time
                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            t.hour, t.min, t.sec);

                    // Remove a configured turn off time
                    if(remove_add == 0)
                    {
                        rc = TimeCfg.remove(t);
                        if(rc == RC_TIMECFG_INVALID_INPUT)
                            fail = true;
                        else if(rc == RC_TIMECFG_NOT_FOUND)
                            time_cfg_found = false;
                        else
                            time_cfg_found = true;
                    }

                    // Configure a new turn off time
                    else if(remove_add == 1)
                    {
                        rc = TimeCfg.add(t, 0);
                        if(rc == RC_TIMECFG_INVALID_INPUT)
                            fail = true;
                        else if(rc == RC_TIMECFG_FOUND)
                            time_cfg_found = true;
                        else if(rc == RC_TIMECFG_FULL)
                            time_cfg_full = true;
                    }

                    // Store it into persistent memory
                    uint8_t tcfg_num = TimeCfg.get_num_time_cfg();
                    t_cfg_time* tcfg = TimeCfg.get_time_cfg();
                    nvm_write(NVM_ADDR_NUM_T_CFG, &tcfg_num, 1);
                    nvm_write(NVM_ADDR_T_CFG, (uint8_t*)tcfg, sizeof(tcfg[0])*MAX_CFG_TIMES);

                    break;
                }

                // Command result fail | success
                if(fail)
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TOFF_INVALID_ARG);
                else
                {
                    // Time to be removed
                    if(remove_add == 0)
                    {
                        // Unset success message | Has not been found message
                        if(time_cfg_found)
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TOFF_UNSET, str_time);
                        else
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TCFG_NOTFOUND);
                    }

                    // Time to be configured
                    else if(remove_add == 1)
                    {
                        // Has been found message | config time full | Set success message
                        if(time_cfg_found)
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TCFG_ALREADY);
                        else if(time_cfg_full == true)
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TCFG_FULL);
                        else
                            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TOFF_SET, str_time);
                    }
                }

                // Send message
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
        }

        // If /timecfg command was received
        else if(strncmp(msg_text, "/timecfg", strlen("/timecfg")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /timecfg received.");

            // Check for clear option
            if(strncmp(msg_text, "/timecfg clear", strlen("/timecfg clear")) == 0)
            {
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TIMECFG_CLEAR);
                TimeCfg.clear();

                // Store it into persistent memory
                uint8_t tcfg_num = TimeCfg.get_num_time_cfg();
                t_cfg_time* tcfg = TimeCfg.get_time_cfg();
                Serial.printf("To store tcfg_num: %" PRIu8 "\n", tcfg_num);
                nvm_write(NVM_ADDR_NUM_T_CFG, &tcfg_num, 1);
                nvm_write(NVM_ADDR_T_CFG, (uint8_t*)tcfg, sizeof(tcfg[0])*MAX_CFG_TIMES);
            }
            else
            {
                // Get configured turn on times
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TIMECFG_1);
                for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
                {
                    // Ignore if not configured
                    if(TimeCfg.get(i, &time_cfg) == false)
                        break;

                    // Ignore if not turn on
                    if(time_cfg.on_off != 1)
                        continue;

                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            time_cfg.t.hour, time_cfg.t.min, time_cfg.t.sec);
                    strncat(str_to_send, str_time, MAX_TIME_SIZE);
                }

                // Get configured turn off times
                strncat(str_to_send, TEXT_TIMECFG_2, strlen(TEXT_TIMECFG_2));
                for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
                {
                    // Ignore if not configured
                    if(TimeCfg.get(i, &time_cfg) == false)
                        break;

                    // Ignore if not turn off
                    if(time_cfg.on_off != 0)
                        continue;

                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            time_cfg.t.hour, time_cfg.t.min, time_cfg.t.sec);
                    strncat(str_to_send, str_time, MAX_TIME_SIZE);
                }
            }

            // Send message
            Serial.println(str_to_send);
            TlgBot.sendMessage(chat_id, str_to_send);
        }

        // If /timezone command was received
        else if(strncmp(msg_text, "/timezone", strlen("/timezone")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /timezone received.");

            // Check for provided arguments
            if(cstr_count_words(msg_text, strlen(msg_text)) < 2)
            {
                // No arguments provided
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TZ_GET);
                if(time_zone < 0)
                    snprintf(str_time, MAX_TIME_SIZE, "-%" PRIi8, ((-1)*time_zone));
                else
                    snprintf(str_time, MAX_TIME_SIZE, "+%" PRIi8, time_zone);
                strncat(str_to_send, str_time, strlen(str_time));
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else
            {
                // Arguments provided
                char* ptr_cmd = msg_text;
                char* ptr_argv;

                // Point to second command argument
                while(1)
                {
                    ptr_argv = strstr(ptr_cmd, " ");
                    if(ptr_argv == NULL)
                    {
                        snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TZ_INVALID_ARG);
                        Serial.println(str_to_send);
                        TlgBot.sendMessage(chat_id, str_to_send);
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Convert argument into integer type
                    if(cstr_string_to_i8(ptr_argv, strlen(ptr_argv), &time_zone, 10) != RC_UTILS_OK)
                    {
                        snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TZ_INVALID_ARG);
                        Serial.println(str_to_send);
                        TlgBot.sendMessage(chat_id, str_to_send);
                        break;
                    }

                    // Set new timezone
                    uint8_t tz = (uint8_t)time_zone;
                    nvm_write(NVM_ADDR_TZ, &tz, 1);
                    NTPclient.setTimeOffset(time_zone*SECONDS_IN_HOUR);
                    TlgBot.set_timezone(time_zone);

                    // Send success message
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TZ_SET);
                    if(time_zone < 0)
                        snprintf(str_time, MAX_TIME_SIZE, "-%" PRIi8, ((-1)*time_zone));
                    else
                        snprintf(str_time, MAX_TIME_SIZE, "+%" PRIi8, time_zone);
                    strncat(str_to_send, str_time, strlen(str_time));
                    Serial.println(str_to_send);
                    TlgBot.sendMessage(chat_id, str_to_send);

                    break;
                }
            }
        }

        // If /time command was received
        else if(strncmp(msg_text, "/time", strlen("/time")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /time received.");

            // Check for provided arguments
            if(cstr_count_words(msg_text, strlen(msg_text)) < 2)
            {
                // No arguments provided
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TIME,
                        NTPclient.getFormattedTime().c_str());
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else
            {
                // Arguments provided
                char* ptr_cmd = msg_text;
                char* ptr_argv;
                bool fail = false;

                // Deep into a new scope
                while(1)
                {
                    // Ignore if unexpected length in provided command
                    if(strlen(ptr_cmd) != strlen("/time XX:YY:ZZ"))
                    {
                        fail = true;
                        break;
                    }

                    // Point to second command argument
                    ptr_argv = strstr(ptr_cmd, " ");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get hour
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.hour), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid hour value
                    if(t.hour > 23)
                    {
                        fail = true;
                        break;
                    }

                    // Point to minutes
                    ptr_argv = strstr(ptr_argv, ":");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get minute
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.min), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid hour value
                    if(t.min > 59)
                    {
                        fail = true;
                        break;
                    }

                    // Point to seconds
                    ptr_argv = strstr(ptr_argv, ":");
                    if(ptr_argv == NULL)
                    {
                        fail = true;
                        break;
                    }
                    ptr_argv = ptr_argv + 1;

                    // Get seconds
                    str_time[0] = ptr_argv[0];
                    str_time[1] = ptr_argv[1];
                    str_time[2] = '\0';

                    // Convert argument into integer type
                    if(cstr_string_to_u8(str_time, strlen(str_time), &(t.sec), 10) != RC_UTILS_OK)
                    {
                        fail = true;
                        break;
                    }

                    // Check for valid second value
                    if(t.sec > 59)
                    {
                        fail = true;
                        break;
                    }

                    // Set new time
                    uint32_t new_time = (t.hour*SECONDS_IN_HOUR) + (t.min*SECONDS_IN_MIN) + t.sec;
                    NTPclient.setEpochTime((unsigned long)new_time);
                    snprintf(str_time, MAX_TIME_SIZE, "%02" PRIi8 ":%02" PRIi8 ":%02" PRIi8 "\n",
                            t.hour, t.min, t.sec);
                    
                    break;
                }

                // Command result
                if(fail)
                {
                    // Send fail message
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TIME_INVALID_ARG);
                    Serial.println(str_to_send);
                    TlgBot.sendMessage(chat_id, str_to_send);
                }
                else
                {
                    // Send success message
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_TIME_SET, str_time);
                    Serial.println(str_to_send);
                    TlgBot.sendMessage(chat_id, str_to_send);
                }
            }
        }

        // If /ntp command was received
        else if(strncmp(msg_text, "/ntp", strlen("/ntp")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /ntp received.");

            if(strncmp(msg_text, "/ntp off", strlen("/ntp off")) == 0)
            {
                ntp_enable = 0;
                nvm_write(NVM_ADDR_NTP_EN, &ntp_enable, 1);
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_OFF);
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else if(strncmp(msg_text, "/ntp on", strlen("/ntp on")) == 0)
            {
                ntp_enable = 1;
                nvm_write(NVM_ADDR_NTP_EN, &ntp_enable, 1);
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_ON);
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else if(strncmp(msg_text, "/ntp status", strlen("/ntp status")) == 0)
            {
                if(ntp_enable == 0)
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_STATUS, "disabled");
                else
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_STATUS, "enabled");
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else if(strncmp(msg_text, "/ntp force", strlen("/ntp force")) == 0)
            {
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP);
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
                if(NTPclient.forceUpdate())
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_OK);
                else
                    snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_FAIL);
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
            else
            {
                snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_NTP_HELP);
                Serial.println(str_to_send);
                TlgBot.sendMessage(chat_id, str_to_send);
            }
        }

        // If /reboot command was received
        else if(strncmp(msg_text, "/reboot", strlen("/reboot")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /reboot received.");

            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_REBOOT);
            Serial.println(str_to_send);
            TlgBot.sendMessage(chat_id, str_to_send);
            TlgBot.getUpdates();
            ESP.restart();
        }

        // If /reboot command was received
        else if(strncmp(msg_text, "/version", strlen("/version")) == 0)
        {
            // Show command reception through Serial
            Serial.println("Command /version received.");

            snprintf(str_to_send, MAX_TLG_MSG_SIZE, TEXT_VERSION, FW_VER);
            Serial.println(str_to_send);
            TlgBot.sendMessage(chat_id, str_to_send);
        }

        // Feed the Watchdog
        yield();
    }
}

/*************************************************************************************************/

// Load parameters from NVM
void nvm_load(void)
{
    uint8_t tz = 0;
    uint8_t tcfg_num = 0;
    t_cfg_time* tcfg = NULL;

    tz = time_zone;
    tcfg_num = TimeCfg.get_num_time_cfg();
    tcfg = TimeCfg.get_time_cfg();

    nvm_read(NVM_ADDR_FTB, &first_time_boot, 1);
    if(first_time_boot != 0)
    {
        Serial.println("Detected first time system boot.");
        first_time_boot = 0;
        nvm_write(NVM_ADDR_FTB, &first_time_boot, 1);
        nvm_write(NVM_ADDR_TZ, &tz, 1);
        nvm_write(NVM_ADDR_NTP_EN, &ntp_enable, 1);
        nvm_write(NVM_ADDR_NUM_T_CFG, &tcfg_num, 1);
        nvm_write(NVM_ADDR_T_CFG, (uint8_t*)tcfg, sizeof(tcfg[0])*MAX_CFG_TIMES);
        return;
    }

    nvm_read(NVM_ADDR_TZ, &tz, 1);
    nvm_read(NVM_ADDR_NTP_EN, &ntp_enable, 1);
    nvm_read(NVM_ADDR_NUM_T_CFG, &tcfg_num, 1);
    nvm_read(NVM_ADDR_T_CFG, (uint8_t*)tcfg, sizeof(tcfg[0])*MAX_CFG_TIMES);

    time_zone = (int8_t)tz;
    TimeCfg.set_num_time_cfg(tcfg_num);
    // Note that "tcfg" is a pointer that already point to TimeCfg.time_cfg data (no need to set)
}

/*************************************************************************************************/

/* WiFi Functions */

// Init WiFi interface
void wifi_init_stat(void)
{
    Serial.println("Initializing TCP-IP adapter...");

    #if defined(WIFI_STATIC_IP) && defined(WIFI_STATIC_GW) && \
    defined(WIFI_STATIC_SUBNET) && defined(WIFI_STATIC_DNS)
        IPAddress staticIP(WIFI_STATIC_IP);
        IPAddress gateway(WIFI_STATIC_GW);
        IPAddress subnet(WIFI_STATIC_SUBNET);
        IPAddress dns(WIFI_STATIC_DNS);
        WiFi.config(staticIP, gateway, subnet, dns);
    #endif

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.println("TCP-IP adapter successfuly initialized.");
    Serial.print("Wifi connecting to SSID: ");
    Serial.println(WIFI_SSID);
}

// WiFi Change Event Handler
bool wifi_handle_connection(void)
{
    static bool wifi_connected = false;
    static bool wifi_has_been_connected = false;

    // Device is not connected
    if(WiFi.status() != WL_CONNECTED)
    {
        // Was connected
        if(wifi_connected)
        {
            Serial.println("WiFi disconnected.");
            wifi_connected = false;
        }

        return false;
    }
    // Device connected
    else
    {
        // Wasn't connected
        if(!wifi_connected)
        {
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());

            // First time connection, initialize NTP client
            if(wifi_has_been_connected == false)
            {
                NTPclient.begin();
                NTPclient.setTimeOffset(time_zone*SECONDS_IN_HOUR);
                TlgBot.set_timezone(time_zone);
                //ota_setup("mysonoff");
            }

            wifi_connected = true;
            wifi_has_been_connected = true;
        }
        else
        {
            if(ntp_enable == 1)
            {
                if(NTPclient.update() == false)
                    Serial.println("NTP update fail");
            }
        }

        return true;
    }
}
