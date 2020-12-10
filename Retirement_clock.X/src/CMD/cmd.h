#define DATETIME &timeinfo
#define BIRTHDAY &b_timeinfo
#define STARTUPDATE &s_timeinfo

#define MAX_YR 65535
#define MIN_YR 0

#include <time.h>

volatile struct tm timeinfo;  // Date
struct tm b_timeinfo;  // Birth date
volatile uint32_t uptime_sec; // Uptime

void CMD_init(void);
int CMD_exec(char *cmd);
int handle_date_cmd(char *method, char *type, char *date, char *time);
void update_date(char date[], char time[], struct tm *selected_tm);
int is_valid_date(char date[]);
int is_valid_time(char time[]);
void CMD_incr_one_sec(); 