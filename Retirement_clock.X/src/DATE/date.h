#define DATETIME &timeinfo
#define BIRTHDAY &b_timeinfo
#define UPTIME &uptime_sec

#define MAX_YR 65535
#define MIN_YR 0

#include <time.h>

// Datetime
volatile struct tm timeinfo;
// Birth date
struct tm b_timeinfo;
// Uptime in seconds
volatile uint32_t uptime_sec;

/**
 * Sets initial date and birth date
 */
void DATE_init(void);
/** 
 * 
 * @param method "SET" or "GET"
 * @param type "DATETIME", "BIRTHDAY", "BACKLIGHT"
 * @param date Date to set in dd.MM.yyyy format
 * @param time Time to set in hh:mm:ss format
 * @return 0 if operation successful
 */
int DATE_handle_date_cmd(char *method, char *type, char *date, char *time);
/**
 * 
 * @param date Date to set in dd.MM.yyyy format
 * @param time Time to set in hh:mm:ss format
 * @param selected_tm Destination tm
 */
void DATE_update_date(char date[], char time[], struct tm *selected_tm);
int DATE_is_valid_date(char date[]);
int DATE_is_valid_time(char time[]);
/**
 * Increments datetime and uptime by 1 second
 */
void DATE_incr_one_sec();
/**
 * Converts uptime seconds to days, hours, minutes and seconds in format:
 * dDhhHmmMssS
 * @param dest Destination string
 */
void DATE_get_uptime(char *dest);
