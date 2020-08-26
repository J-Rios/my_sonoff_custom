
#include <inttypes.h>

// Telegram Bot Reply Keyboard Markup
//const char keyboard[] = 
//    "["
//        "[\"/on\",\"/off\"],"
//        "[\"/status\"]"
//    "]";

// Telegram Bot /start text message
const char TEXT_START[] = 
    "Hello, im a Bot running in a Sonoff Device (ESP8285 microcontroller) that let you control "
    "this device.\n"
    "\n"
    "Check /help command to see how to use me.";

// Telegram Bot /help text message
const char TEXT_HELP[] = 
    "Available Commands:\n"
    "\n"
    "/start - Shows start text and request Bot Keyboard.\n"
    "/help - Shows actual text.\n"
    "/on - Turn on the Relay.\n"
    "/off - Turn off the Relay.\n"
    "/status - Shows actual Relay status.\n"
    "/time - Get/Set current device time.\n"
    "/timeon - Set a time to device automatically turn on the Relay.\n"
    "/timeoff - Set a time to device automatically turn on the Relay.\n"
    "/timecfg - Shows/clear all configured automatic turn on and off Relay times.\n"
    "/timezone - Get/Set system time zone.\n"
    "/ntp - Enable/Disable/Force update device time from NTP Server.\n"
    "/reboot - Force a system reboot.";

// Telegram Bot /on message
const char TEXT_ON[] = 
    "Relay turned on.";

// Telegram Bot /off message
const char TEXT_OFF[] = 
    "Relay turned off.";

// Telegram Bot /status message on
const char TEXT_STATUS_ON[] = 
    "The Relay is on.";

// Telegram Bot /status message off
const char TEXT_STATUS_OFF[] = 
    "The Relay is off.";

// Telegram Bot /time message (get)
const char TEXT_TIME[] = 
    "Current Time:\n%s";

// Telegram Bot /time message (set)
const char TEXT_TIME_SET[] = 
    "New Time has been set:\n%s";

// Telegram Bot /time message (invalid)
const char TEXT_TIME_INVALID_ARG[] = 
    "You need to provide a valid time.\n"
    "Examples:\n"
    "/time\n"
    "/time 00:00:00\n"
    "/time 14:35:20\n";

// Telegram Bot /timeoff and /timeon message config not found
const char TEXT_TCFG_NOTFOUND[] = 
    "Time to remove not found.";

// Telegram Bot /timeoff and /timeon message config found
const char TEXT_TCFG_ALREADY[] = 
    "This time is already configured.";

// Telegram Bot /timeoff message (set) full
const char TEXT_TCFG_FULL[] = 
    "Configurable turn on/off times slots are full. Remove someone first.";

// Telegram Bot /timeon message (get)
const char TEXT_TON_GET[] = 
    "Configured turn on times:\n";

// Telegram Bot /timeon message (set)
const char TEXT_TON_SET[] = 
    "Successfully configured new turn on time:\n%s";

// Telegram Bot /timeon message (unset)
const char TEXT_TON_UNSET[] = 
    "Successfully removed configured turn on time:\n%s";

// Telegram Bot /timeon message (invalid)
const char TEXT_TON_INVALID_ARG[] = 
    "You need to provide a valid time.\n"
    "Examples:\n"
    "/timeon\n"
    "/timeon set 23:20:00\n"
    "/timeon unset 23:20:00\n";

// Telegram Bot /timeoff message (get)
const char TEXT_TOFF_GET[] = 
    "Configured turn off times:\n";

// Telegram Bot /timeoff message (set)
const char TEXT_TOFF_SET[] = 
    "Successfully configured new turn off time:\n%s";

// Telegram Bot /timeoff message (unset)
const char TEXT_TOFF_UNSET[] = 
    "Successfully removed configured turn off time:\n%s";

// Telegram Bot /timeoff message (invalid)
const char TEXT_TOFF_INVALID_ARG[] = 
    "You need to provide a valid time.\n"
    "Examples:\n"
    "/timeoff\n"
    "/timeoff set 01:00:00\n"
    "/timeoff unset 01:00:00\n";

// Telegram Bot /timecfg clear message
const char TEXT_TIMECFG_CLEAR[] = 
    "All configured times for turn on/off cleared.";

// Telegram Bot /timecfg message 1
const char TEXT_TIMECFG_1[] = 
    "Turn ON times:\n";

// Telegram Bot /timecfg message 2
const char TEXT_TIMECFG_2[] = 
    "\nTurn OFF times:\n";

// Telegram Bot /timezone message (get)
const char TEXT_TZ_GET[] = 
    "Actual Time Zone:\nUTC";

// Telegram Bot /timezone message (set)
const char TEXT_TZ_SET[] = 
    "New Time Zone has been set:\nUTC";

// Telegram Bot /timezone message (invalid)
const char TEXT_TZ_INVALID_ARG[] = 
    "You need to provide a valid time zone.\n"
    "Examples:\n"
    "/timezone\n"
    "/timezone +0\n"
    "/timezone +2\n"
    "/timezone -1\n";

// Telegram Bot /ntp off message
const char TEXT_NTP_OFF[] = 
    "NTP syncronization disabled.";

// Telegram Bot /ntp on message
const char TEXT_NTP_ON[] = 
    "NTP syncronization enabled.";

// Telegram Bot /ntp status message
const char TEXT_NTP_STATUS[] = 
    "Current NTP syncronization config:\n%s";

// Telegram Bot /ntp message first text
const char TEXT_NTP[] = 
    "Updating date from NTP Server...";

// Telegram Bot /ntp help message
const char TEXT_NTP_HELP[] = 
    "You need to provide a valid option:\n"
    "/ntp status\n"
    "/ntp on\n"
    "/ntp off\n"
    "/ntp force\n";

// Telegram Bot /ntp message success
const char TEXT_NTP_OK[] = 
    "Date succesfully updated.";

// Telegram Bot /ntp message fail
const char TEXT_NTP_FAIL[] = 
    "NTP update fail.";

// Telegram Bot /reboot message
const char TEXT_REBOOT[] = 
    "Rebooting the system...";

// Telegram Bot /version message
const char TEXT_VERSION[] = 
    "Current system version:\n%s\n";
