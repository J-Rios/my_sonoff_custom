
// Firmware Version
#define FW_VER "1.0.0"

// WiFi Parameters
#define WIFI_SSID "mynet1234"
#define WIFI_PASS "mypassword1234"
//#define WIFI_STATIC_IP 192, 168, 1, 240
//#define WIFI_STATIC_GW 192, 168, 1, 1
//#define WIFI_STATIC_SUBNET 255, 255, 255, 0
//#define WIFI_STATIC_DNS 8, 8, 8, 8
#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63

// Telegram Bot Token (Get from Botfather)
#define TLG_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// Telegram group chat_id where is allowed to interact with the Bot
// The Bot will be "isolated" to be use just by this group member
#define TLG_GROUP_ID "-123456789"

// Enable Bot debug level (0 - None; 1 - Bot Level; 2 - Bot+HTTPS Level)
#define DEBUG_LEVEL_UTLGBOT 0

// Non-Volatile-Memory (virtual "EEPROM") usage size (bytes)
#define NVM_SIZE_TO_USE 256

// Non-Volatile-Memory data addresses
#define NVM_ADDR_FTB       0x0000
#define NVM_ADDR_TZ        0x0001
#define NVM_ADDR_NTP_EN    0x0002
                           // ... RFU (Reserved for Future Use)
#define NVM_ADDR_NUM_T_CFG 0x0010
#define NVM_ADDR_T_CFG     0x0011

// Time constants
#define SECONDS_IN_MIN 60
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_DAY 86400
#define MILLIS_IN_SEC 1000

// NTP Server Address
#define NTP_SERVER_ADDR "pool.ntp.org"

//NTP server connection to update and sync date once each 60 minutes
#define NTP_UPDATE_EACH_MIN (60 * SECONDS_IN_MIN * MILLIS_IN_SEC)

// Telegram message text max length
#define MAX_TLG_MSG_SIZE 128

// Time string max length ("XX:YY:ZZ\n")
#define MAX_TIME_SIZE 10
