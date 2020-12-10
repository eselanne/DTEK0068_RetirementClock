#define DATETIME &timeinfo
#define BIRTHDAY &b_timeinfo
#define STARTUPDATE &s_timeinfo

struct tm timeinfo;  // Date
struct tm b_timeinfo;  // Birth date
struct tm s_timeinfo;  // Start date

void CMD_init(void);
int CMD_exec(char *cmd);
int handle_date_cmd(char *method, char *type, char *date, char *time);
void update_date(char date[], char time[], struct tm *selected_tm);
int is_ok_format(char date[], char time[]);