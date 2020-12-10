#include <time.h>
#include <string.h>
#include <stdio.h>
#include "cmd.h"
#include "../USART/usart.h"
#include "../RTC/rtc.h"


struct tm timeinfo;  // Date
struct tm b_timeinfo;  // Birth date
struct tm s_timeinfo = { 0 };  // Start date

void CMD_init()
{
    handle_date_cmd("SET", "DATETIME", "06.09.1969", "04:20:00");
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
        if(is_ok_format(date, time) != 0)
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
    selected_tm->tm_sec = atoi(sec_c);
    selected_tm->tm_min = atoi(min_c);
    selected_tm->tm_hour = atoi(hour_c);
    selected_tm->tm_mday = atoi(day_c);
    selected_tm->tm_mon = atoi(month_c) - 1;
    selected_tm->tm_year = atoi(year_c) - 1900;
}

int is_ok_format(char date[], char time[])
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
    uint32_t year = atoi(year_c);
    uint8_t month = atoi(month_c);
    uint8_t day = atoi(day_c);
    uint8_t hour = atoi(hour_c);
    uint8_t min = atoi(min_c);
    uint8_t sec = atoi(sec_c);
    if(!(0 <= year && year <= 9999) ||
        !(0 <= month && month <= 12) ||
        !(0 <= day && day <= 31) ||
        !(0 <= hour && hour <= 59) ||
        !(0 <= min && min <= 59) ||
        !(0 <= sec && sec <= 59))
    {
        return 1;
    }
    return 0;
}