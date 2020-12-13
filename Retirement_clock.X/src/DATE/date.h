#define DATETIME &timeinfo
#define BIRTHDAY &b_timeinfo
#define RETIREMENT &r_timeinfo
#define UPTIME &uptime_sec

#define MAX_YR 65535
#define MIN_YR 0

#define RET_AGE 65

#include <time.h>

// Datetime
volatile struct tm timeinfo;
// Birth date
volatile struct tm b_timeinfo;
// Retirement date
volatile struct tm r_timeinfo;
// Uptime in seconds
volatile uint32_t uptime_sec;

/**
 * Sets initial date, birth date and retirement date
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
 * Gets uptime counter in format: dDhhHmmMssS
 * @param dest Destination string
 */
void DATE_get_uptime(char *dest);
/**
 * Gets countdown to retirement in format: dDhhHmmMssS
 * @param dest Destination string
 */
void DATE_get_countdown(char *dest);
/**
 * Retirement = birthday.years + RET_AGE
 * @param r_timeinfo Pointer to retirement timeinfo
 */
void DATE_calc_ret_date(struct tm *dest_tm);
void DATE_sec_to_countdown_format(uint32_t seconds, char *dest);
/**
 * Difference between two datetimes in seconds
 * @param start
 * @param end
 * @return Difference in seconds
 */
uint32_t DATE_diff_in_seconds(struct tm *start, struct tm *end);
/**
 * Calculates new retirement date based on birth date. Also updates countdown
 * to retirement based on datetime.
 */
void DATE_update_ret_date(void);