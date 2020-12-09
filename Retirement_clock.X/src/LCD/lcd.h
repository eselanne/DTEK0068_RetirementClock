#define DATA_PORT PORTD_OUT
#define DATA_DDR PORTD_DIR
#define CNTRL_PORT PORTB_OUT
#define CNTRL_DDR PORTB_DIR
#define RS_PIN 4
#define RW_PIN 5 // doesn't matter, its ground
#define ENABLE_PIN 3

void LCD_send_command(unsigned char cmnd);
void LCD_send_data(unsigned char data);
void LCD_init(void);
void LCD_goto(unsigned char y, unsigned char x);
void LCD_print(char *string);
void LCD_blink(void);
void LCD_clear(void);