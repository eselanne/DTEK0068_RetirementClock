#include <time.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "cmd.h"
#include "../USART/usart.h"
#include "../RTC/rtc.h"


volatile struct tm timeinfo;  // Date
struct tm b_timeinfo;  // Birth date
volatile uint32_t uptime_sec = 0; // Uptime

void CMD_init()
{    
    cli();
    timeinfo.tm_sec = 0;
    timeinfo.tm_min = 20;
    timeinfo.tm_hour = 4;
    timeinfo.tm_mday = 6;
    timeinfo.tm_mon = 9 - 1;
    timeinfo.tm_year = 2020 - 1900;
    sei();
}

void CMD_incr_one_sec()
{
    // Handle time overflow
    cli();
    uptime_sec++;
    timeinfo.tm_sec++;
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

int CMD_exec(char *cmd)
{    
    char args[16][32];
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t ctr = 0;

    for(i = 0; i <= (strlen(cmd)); i++)
    {
        // if space or NULL found, assign NULL into args[ctr]
        if((cmd[i] == ' ') || (cmd[i] == '\0'))
        {
            args[ctr][j] = '\0';
            ctr++;  //for next word
            j = 0;  //for next word, init index to 0
        }
        else
        {
            args[ctr][j] = cmd[i];
            j++;
        }
    }

    if((strcmp(args[1], "DATETIME") == 0) || (strcmp(args[1], "BIRTHDAY") == 0))
    {
        if(handle_date_cmd(args[0], args[1], args[2], args[3]) == 0)
        {
            return 0;
        }
    }

    if(strcmp(args[1], "BACKLIGHT") == 0)
    {
        if(strcmp(args[0], "GET") == 0)
        {
            char msg_str[100];
            snprintf(msg_str, sizeof(msg_str), "LCD backlight duration: %d seconds\r\n", backlight_duration);
            USART0_sendString(msg_str);
            return 0;
        }
        if(strcmp(args[0], "SET") == 0)
        {
            int16_t duration = atoi(args[2]);
            backlight_duration = duration > 0 ? duration : 5;
            return 0;
        }
    }
   
    USART0_sendString("Invalid command\r\n");
    return 1;
}

int handle_date_cmd(char *method, char *type, char *date, char *time)
{
    struct tm *selected_tm;
    if(strcmp(type, "DATETIME") == 0)
    {
        selected_tm = &timeinfo;
    }
    else if(strcmp(type, "BIRTHDAY") == 0)
    {
        selected_tm = &b_timeinfo;
    }
    else
    {
        return 1;
    }
    if(strcmp(method, "GET") == 0)
    {
        char msg_str[100];
        strftime(msg_str, sizeof(msg_str), "%d.%m.%Y %H:%M:%S\r\n", selected_tm);
        USART0_sendString(msg_str);
        return 0;
    }
    if(strcmp(method, "SET") == 0)
    {
        if(!is_valid_date(date))
        {
            return 1;
        }
        if((strcmp(type, "DATETIME") == 0) && !is_valid_time(time))
        {
            return 1;
        }
        update_date(date, time, selected_tm);        
        return 0;
    }
    return 1;
}

void update_date(char date[], char time[], struct tm *selected_tm)
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

int is_valid_date(char date[])
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

int is_valid_time(char time[])
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