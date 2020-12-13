#define DATA_PORT PORTD_OUT
#define DATA_DDR PORTD_DIR
#define CNTRL_PORT PORTB_OUT
#define CNTRL_DDR PORTB_DIR
#define RS_PIN 4
#define RW_PIN 5 // doesn't matter, its ground
#define ENABLE_PIN 3

/**
 * Rotatable LCD views
 */
typedef enum 
{
    CLOCK_VIEW = 1,
    COUNTDOWN_VIEW = 2,
    UPTIME_VIEW = 3
} LCD_views;

/**
 * The active view
 */
LCD_views LCD_view;

void LCD_send_command(unsigned char cmnd);
void LCD_send_data(unsigned char data);
void LCD_init(void);
/**
 * Moves cursor to given point
 * @param y Row, possible values 1 and 2
 * @param x Column, possible values 1 to 16
 */
void LCD_goto(unsigned char y, unsigned char x);
void LCD_print(char *string);
void LCD_clear(void);
/**
 * Prints the current active view to LCD
 */
void LCD_update_view(void);
