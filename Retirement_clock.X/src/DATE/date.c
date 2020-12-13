#include <time.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "date.h"
#include "../USART/usart.h"
#include "../RTC/rtc.h"


volatile struct tm timeinfo;  // Datetime
volatile struct tm b_timeinfo;  // Birth date
volatile struct tm r_timeinfo; // Retirement date
volatile uint32_t uptime_sec = 0; // Uptime
volatile uint32_t time_to_ret_sec; // Uptime

void DATE_init()
{    
    cli();
    // Init datetime
    timeinfo.tm_sec = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_hour = 0;
    timeinfo.tm_mday = 1; // TODO tm_mday != 1
    timeinfo.tm_mon = 3 - 1;
    timeinfo.tm_year = 2020 - 1900;
    // Init birth date
    b_timeinfo.tm_sec = 0;
    b_timeinfo.tm_min = 0;
    b_timeinfo.tm_hour = 0;
    b_timeinfo.tm_mday = 4;
    b_timeinfo.tm_mon = 3 - 1;
    b_timeinfo.tm_year = 1997 - 1900;
    // Init retirement
    DATE_update_ret_date();    
    sei();
}

void DATE_incr_one_sec()
{
    cli();
    uptime_sec++;
    time_to_ret_sec--;
    timeinfo.tm_sec++;
    // Handle time overflow
    if(timeinfo.tm_sec >= 60)
    {
        timeinfo.tm_sec = 0;
        timeinfo.tm_min++;
    }
    if(timeinfo.tm_min >= 60)
    {
        timeinfo.tm_min = 0;
        timeinfo.tm_hour++;
    }
    if(timeinfo.tm_hour >= 24)
    {
        timeinfo.tm_hour = 0;
        timeinfo.tm_mday++;
    }
    
    // Handle month day overflow
    if(timeinfo.tm_mon + 1 == 2) // Is February
    {
        if(is_leap_year(timeinfo.tm_year + 1900))
        {
            if(timeinfo.tm_mday > 29)
            {
                timeinfo.tm_mday = 1;
                timeinfo.tm_mon++;
            }
        }
        else if(timeinfo.tm_mday > 28)
        {
            timeinfo.tm_mday = 1;
            timeinfo.tm_mon++;
        }
    }
    else // Not February
    {
        if(timeinfo.tm_mday > 31)
        {
            timeinfo.tm_mday = 1;
            timeinfo.tm_mon++;
        }        
        // Months w/ 30 days
        switch (timeinfo.tm_mon + 1)
        {
            case 4: // Apr
            case 6: // Jun
            case 9: // Sep
            case 11: // Nov
                if(timeinfo.tm_mday > 30)
                {
                    timeinfo.tm_mday = 1;
                    timeinfo.tm_mon++;
                }
                break;
        }
    }
    
    // Handle month overflow
    if((timeinfo.tm_mon + 1) > 12)
    {
        // January first day
        timeinfo.tm_mday = 1;
        timeinfo.tm_mon = 0;
        timeinfo.tm_year++;
    }
    sei();
}

int DATE_handle_date_cmd(char *method, char *type, char *date, char *time)
{
    struct tm *selected_tm;
    if(strcmp(type, "DATETIME") == 0)
    {
        selected_tm = (struct tm*) &timeinfo;
    }
    else if(strcmp(type, "BIRTHDAY") == 0)
    {
        selected_tm = (struct tm*) &b_timeinfo;
    }
    else
    {
        return 1;
    }
    if(strcmp(method, "GET") == 0)
    {
        char msg_str[100];
        strftime(msg_str, sizeof(msg_str),
                "%d.%m.%Y %H:%M:%S\r\n", selected_tm);
        USART0_sendString(msg_str);
        return 0;
    }
    if(strcmp(method, "SET") == 0)
    {
        if(!DATE_is_valid_date(date))
        {
            return 1;
        }
        if((strcmp(type, "DATETIME") == 0) && !DATE_is_valid_time(time))
        {
            return 1;
        }
        DATE_update_date(date, time, selected_tm);
        DATE_update_ret_date();     
        return 0;
    }
    return 1;
}

void DATE_update_date(char date[], char time[], struct tm *selected_tm)
{
    char* year_c = malloc(4);
    char* month_c = malloc(2);
    char* day_c = malloc(2);
    char* hour_c = malloc(2);
    char* min_c = malloc(2);
    char* sec_c = malloc(2);
    strncpy(year_c, date + 6, 4);
    strncpy(month_c, date + 3, 2);
    strncpy(day_c, date, 2);
    strncpy(hour_c, time, 2);
    strncpy(min_c, time + 3, 2);
    strncpy(sec_c, time + 6, 2);
    cli();
    selected_tm->tm_sec = atoi(sec_c);
    selected_tm->tm_min = atoi(min_c);
    selected_tm->tm_hour = atoi(hour_c);
    selected_tm->tm_mday = atoi(day_c);
    selected_tm->tm_mon = atoi(month_c) - 1;
    selected_tm->tm_year = atoi(year_c) - 1900;
    sei();
}

int DATE_is_valid_date(char date[])
{
    char* year_c = malloc(4);
    char* month_c = malloc(2);
    char* day_c = malloc(2);
    strncpy(year_c, date + 6, 4);
    strncpy(month_c, date + 3, 2);
    strncpy(day_c, date, 2);
    uint16_t year = atoi(year_c);
    uint8_t month = atoi(month_c);
    uint8_t day = atoi(day_c);

    //check range of year,month and day
    if ((year > MAX_YR) || (year < MIN_YR))
    {
        return 0;
    }
    if ((month < 1) || (month > 12))
    {
        return 0;
    }
    if ((day < 1) || (day > 31))
    {
        return 0;
    }

    //Handle feb days in leap year
    if (month == 2)
    {
        if (is_leap_year(year))
        {
            return (day <= 29);
        }
        return (day <= 28);
    }
    
    //handle months which has only 30 days
    if ((month == 4) || (month == 6) ||
            (month == 9) || (month == 11))
    {
        return (month <= 30);
    }
    return 1;
}

int DATE_is_valid_time(char time[])
{
    char* hour_c = malloc(2);
    char* min_c = malloc(2);
    char* sec_c = malloc(2);
    strncpy(hour_c, time, 2);
    strncpy(min_c, time + 3, 2);
    strncpy(sec_c, time + 6, 2);
    uint8_t hour = atoi(hour_c);
    uint8_t min = atoi(min_c);
    uint8_t sec = atoi(sec_c);
    if(!((0 <= hour) && (hour <= 23)))
    {
        return 0;
    }
    if(!((0 <= min) && (min <= 59)))
    {
        return 0;
    }
    if(!((0 <= sec) && (sec <= 59)))
    {
        return 0;
    }
    return 1;
}

void DATE_get_uptime(char *dest)
{
    if(uptime_sec < 1)
    {
        return;
    }    
    cli();
    DATE_sec_to_countdown_format(uptime_sec, dest);
    sei();
}

void DATE_calc_ret_date(struct tm *dest_tm)
{
    dest_tm->tm_sec = /*b_timeinfo.tm_sec*/ 0;
    dest_tm->tm_min = /*b_timeinfo.tm_min*/ 0;
    dest_tm->tm_hour = /*b_timeinfo.tm_hour*/ 0;
    dest_tm->tm_mday = b_timeinfo.tm_mday;
    dest_tm->tm_mon = b_timeinfo.tm_mon;
    dest_tm->tm_year = b_timeinfo.tm_year + RET_AGE;
}

void DATE_get_countdown(char *dest)
{    
    cli();
    DATE_sec_to_countdown_format(time_to_ret_sec, dest);
    sei();
}

void DATE_sec_to_countdown_format(uint32_t seconds, char *dest)
{
    uint32_t remaining_seconds = seconds;
    uint16_t days = remaining_seconds / 86400;
    remaining_seconds %= 86400;
    uint8_t hours = (remaining_seconds / 3600);
    remaining_seconds %= 3600;
    uint8_t minutes = (remaining_seconds / 60);
    remaining_seconds %= 60;    
    snprintf(dest, 100, "%02dD%02dH%02dM%02dS", 
            (int)days, (int)hours, (int)minutes, (int)remaining_seconds);
}

uint32_t DATE_diff_in_seconds(struct tm *start, struct tm *end)
{  
    uint32_t seconds = 0;
    uint8_t is_first_loop = 1;
    uint8_t is_first_month = 1;
    uint16_t year = start->tm_year + 1900;
    // Loop years
    for(; year <= (end->tm_year + 1900); year++)
    {
        uint8_t is_last_loop = year == (end->tm_year + 1900);
        uint8_t month = is_first_loop ? (start->tm_mon + 1) : 1;
        uint8_t last_month = is_last_loop ? end->tm_mon : 12;
        // Loop "full" months in a year
        // First month is most likely not full, so it's an exception
        for(; month <= last_month; month++)
        {
            uint8_t days_in_month = 31;
            if(month == 2)
            {
                days_in_month = is_leap_year(year) ? 29 : 28;
            }
            if((month == 4) || (month == 6) || (month == 9) || (month == 11))
            {
                days_in_month = 30;
            }
            seconds += days_in_month * 86400;
            seconds -= is_first_month ? ((start->tm_mday - 1) * 86400) : 0;
            is_first_month = 0;
        }
        // Add remaining seconds
        if(is_last_loop)
        {
            seconds += (end->tm_mday - 1) * 86400;
            seconds += end->tm_hour * 3600;
            seconds += end->tm_min * 60;
            seconds += end->tm_sec;
        }
        is_first_loop = 0;
    }
    return seconds;
}

void DATE_update_ret_date()
{
    DATE_calc_ret_date((struct tm*) RETIREMENT);
    time_to_ret_sec = DATE_diff_in_seconds((struct tm*) DATETIME,
                                            (struct tm*) RETIREMENT);
}