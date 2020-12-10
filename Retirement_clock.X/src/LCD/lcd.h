#define DATA_PORT PORTD_OUT
#define DATA_DDR PORTD_DIR
#define CNTRL_PORT PORTB_OUT
#define CNTRL_DDR PORTB_DIR
#define RS_PIN 4
#define RW_PIN 5 // doesn't matter, its ground
#define ENABLE_PIN 3

// TODO: typedef enum... (coding standard 5.1 b)
enum LCD_views
{
    CLOCK_VIEW = 1,
    COUNTDOWN_VIEW = 2,
    UPTIME_VIEW = 3
} LCD_view;

void LCD_send_command(unsigned char cmnd);
void LCD_send_data(unsigned char data);
void LCD_init(void);
void LCD_goto(unsigned char y, unsigned char x);
void LCD_print(char *string);
void LCD_blink(void);
void LCD_clear(void);
void LCD_set_view(enum LCD_views view, struct tm *timeinfo);
void LCD_update_view();
