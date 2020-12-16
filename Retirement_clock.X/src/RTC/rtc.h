void RTC_init(void);
void LCD_rotate_views(void);


volatile uint16_t backlight_duration;
volatile uint16_t backlight_counter;

inline void RTC_set_bl_duration(uint16_t duration_seconds)
{
    backlight_duration = duration_seconds;
} 