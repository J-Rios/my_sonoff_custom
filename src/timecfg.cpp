
#include "timecfg.h"

/*************************************************************************************************/

CTimeCfg::CTimeCfg(void)
{
    num_time_cfg = 0;
    clear();
}

CTimeCfg::~CTimeCfg(void)
{}

/*************************************************************************************************/

void CTimeCfg::clear(void)
{
    num_time_cfg = 0;
    for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
    {
        time_cfg[i].on_off = -1;
        time_cfg[i].t.hour = 0;
        time_cfg[i].t.min = 0;
        time_cfg[i].t.sec = 0;
    }
}

int8_t CTimeCfg::add(const t_simple_time t, const int8_t on_off)
{
    uint8_t i = 0;
    uint8_t ii = 0;

    // Check valid arguments
    if((t.hour > 23) || (t.min > 59) || (t.sec > 59))
        return RC_TIMECFG_INVALID_INPUT;
    if((on_off != -1) && (on_off != 0) && (on_off != 1))
        return RC_TIMECFG_INVALID_INPUT;

    // Check if it is full
    if(num_time_cfg >= MAX_CFG_TIMES)
        return RC_TIMECFG_FULL;

    // Get ordered position to store the time (get previous time index)
    i = 0;
    while(i < MAX_CFG_TIMES)
    {
        // Check if element is available
        if(time_cfg[i].on_off == -1)
            break;

        // Check if element is already in
        if((time_cfg[i].t.hour == t.hour) && (time_cfg[i].t.min == t.min) &&
        (time_cfg[i].t.sec == t.sec))
            return RC_TIMECFG_FOUND;

        // Check if element time is less than configured one
        if(t.hour < time_cfg[i].t.hour)
            break;
        else if(t.hour == time_cfg[i].t.hour)
        {
            if(t.min < time_cfg[i].t.min)
                break;
            else if(t.min == time_cfg[i].t.min)
            {
                if(t.sec < time_cfg[i].t.sec)
                    break;
            }
        }

        i = i + 1;
    }

    // Order (shift right elements)
    ii = MAX_CFG_TIMES - 1;
    while(ii > i)
    {
        time_cfg[ii].on_off = time_cfg[ii-1].on_off;
        time_cfg[ii].t.hour = time_cfg[ii-1].t.hour;
        time_cfg[ii].t.min = time_cfg[ii-1].t.min;
        time_cfg[ii].t.sec = time_cfg[ii-1].t.sec;
        ii = ii - 1;
    }

    // Add new element
    time_cfg[i].on_off = on_off;
    time_cfg[i].t.hour = t.hour;
    time_cfg[i].t.min = t.min;
    time_cfg[i].t.sec = t.sec;

    num_time_cfg = num_time_cfg + 1;

    return RC_TIMECFG_OK;
}

int8_t CTimeCfg::remove(const t_simple_time t)
{
    int16_t found_i = -1;

    // Check valid arguments
    if((t.hour > 23) || (t.min > 59) || (t.sec > 59))
        return RC_TIMECFG_INVALID_INPUT;

    // Check if it is empty
    if(num_time_cfg == 0)
        return RC_TIMECFG_NOT_FOUND;

    for(uint8_t i = 0; i < MAX_CFG_TIMES; i++)
    {
        // Found it
        if((time_cfg[i].t.hour == t.hour) && (time_cfg[i].t.min == t.min) &&
        (time_cfg[i].t.sec == t.sec))
        {
            found_i = i;
            break;
        }
    }

    // Not found
    if(found_i == -1)
        return RC_TIMECFG_NOT_FOUND;

    // Order shifting left each element
    for(uint8_t i = found_i; i < MAX_CFG_TIMES; i++)
    {
        if(i+1 >= MAX_CFG_TIMES)
        {
            time_cfg[i].on_off = -1;
            time_cfg[i].t.hour = 0;
            time_cfg[i].t.min = 0;
            time_cfg[i].t.sec = 0;
            break;
        }

        time_cfg[i].on_off = time_cfg[i+1].on_off;
        time_cfg[i].t.hour = time_cfg[i+1].t.hour;
        time_cfg[i].t.min = time_cfg[i+1].t.min;
        time_cfg[i].t.sec = time_cfg[i+1].t.sec;
    }

    num_time_cfg = num_time_cfg - 1;

    return RC_TIMECFG_OK;
}

bool CTimeCfg::get(const uint8_t i, t_cfg_time* t_cfg)
{
    if(i >= num_time_cfg)
        return false;

    t_cfg->on_off = time_cfg[i].on_off;
    t_cfg->t.hour = time_cfg[i].t.hour;
    t_cfg->t.min = time_cfg[i].t.min;
    t_cfg->t.sec = time_cfg[i].t.sec;

    return true;
}

t_cfg_time* CTimeCfg::get_time_cfg(void)
{
    return &time_cfg[0];
}

uint8_t CTimeCfg::get_num_time_cfg(void)
{
    return num_time_cfg;
}

void CTimeCfg::set_num_time_cfg(uint8_t _num_time_cfg)
{
    num_time_cfg = _num_time_cfg;
}

/*************************************************************************************************/

