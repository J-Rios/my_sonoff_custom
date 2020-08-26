
#ifndef __TIMECFG_H
#define __TIMECFG_H

#ifdef __cplusplus
    extern "C" {
#endif

/*************************************************************************************************/

#include <inttypes.h>

/*************************************************************************************************/

// Maximum number of configured times to program automatically turn on/off relay in a day
#define MAX_CFG_TIMES 10 // Note that UTLGBOT_MEMORY_LEVEL limit max msg text for /timecfg

/*************************************************************************************************/

#pragma pack(push, 1)
    typedef struct t_simple_time
    {
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
    } t_simple_time;

    typedef struct t_cfg_time
    {
        t_simple_time t;
        int8_t on_off;
    } t_cfg_time;
#pragma pack(pop)

#define RC_TIMECFG_OK 0
#define RC_TIMECFG_FULL 1
#define RC_TIMECFG_FOUND 2
#define RC_TIMECFG_NOT_FOUND 3
#define RC_TIMECFG_BAD -1
#define RC_TIMECFG_INVALID_INPUT -2

/*************************************************************************************************/

class CTimeCfg
{
    public:
        CTimeCfg(void);
        ~CTimeCfg();
        void clear(void);
        int8_t add(const t_simple_time t, const int8_t on_off);
        int8_t remove(const t_simple_time t);
        bool get(const uint8_t i, t_cfg_time* t_cfg);
        t_cfg_time* get_time_cfg(void);
        uint8_t get_num_time_cfg(void);
        void set_num_time_cfg(uint8_t _num_time_cfg);

    private:
        uint8_t num_time_cfg;
        t_cfg_time time_cfg[MAX_CFG_TIMES];
};

/*************************************************************************************************/

/*************************************************************************************************/

#ifdef __cplusplus
    }
#endif

#endif /* __TIMECFG_H */
