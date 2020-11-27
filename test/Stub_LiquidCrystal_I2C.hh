// Copyright 2020 Brett M Leedy
/**
 * LCD stub class
 */
#ifndef TEST_STUB_LIQUIDCRYSTAL_I2C_HH_
#define TEST_STUB_LIQUIDCRYSTAL_I2C_HH_

#include <inttypes.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

class LiquidCrystal_I2C {
 public:
  LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols,
                      uint8_t lcd_rows) {}
  virtual void begin(uint8_t cols, uint8_t rows,
                      uint8_t charsize = LCD_5x8DOTS ) {}
  virtual void clear() {}
  virtual void home() {}
  virtual void noDisplay() {}
  virtual void display() {}
  virtual void noBlink() {}
  virtual void blink() {}
  virtual void noCursor() {}
  virtual void cursor() {}
  virtual void scrollDisplayLeft() {}
  virtual void scrollDisplayRight() {}
  virtual void printLeft() {}
  virtual void printRight() {}
  virtual void leftToRight() {}
  virtual void rightToLeft() {}
  virtual void shiftIncrement() {}
  virtual void shiftDecrement() {}
  virtual void noBacklight() {}
  virtual void backlight() {}
  virtual void autoscroll() {}
  virtual void noAutoscroll() {}
  virtual void createChar(uint8_t, uint8_t[]) {}
  virtual void createChar(uint8_t location, const char *charmap) {}
  virtual void print(char * toprint) {}
  virtual void print(const char * toprint) {}
  virtual void print(uint16_t toprint) {}
  virtual void print(uint16_t toprint, uint8_t precision) {}
  // Example: const char bell[8] PROGMEM =
  //   {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};

  virtual void setCursor(uint8_t, uint8_t) {}
#if defined(ARDUINO) && ARDUINO >= 100
  virtual size_t write(uint8_t) {}
#else
  virtual void write(uint8_t) {}
#endif
  virtual void command(uint8_t) {}
  virtual void init() {}
  virtual void oled_init() {}

// compatibility API function aliases
virtual void blink_on() {}   // alias for blink()
virtual void blink_off() {}  // alias for noBlink()
virtual void cursor_on() {}  // alias for cursor()
virtual void cursor_off() {}  // alias for noCursor()
virtual void setBacklight(uint8_t new_val) {}  // alias for backlight() a
                                               // and nobacklight()
virtual void load_custom_character(uint8_t char_num,
                                   uint8_t *rows) {}  // alias for createChar()
virtual void printstr(const char[]) {}

// Unsupported API functions (not implemented in this library)
uint8_t status() { return 0;}
virtual void setContrast(uint8_t new_val) {}
uint8_t keypad() {return 0;}
virtual void setDelay(int, int) {}
virtual void on() {}
virtual void off() {}
uint8_t init_bargraph(uint8_t graphtype) {return 0;}
virtual void draw_horizontal_graph(uint8_t row, uint8_t column,
                                    uint8_t len,  uint8_t pixel_col_end) {}
virtual void draw_vertical_graph(uint8_t row, uint8_t column,
                                    uint8_t len, uint8_t pixel_col_end) {}
};
#endif  // TEST_STUB_LIQUIDCRYSTAL_I2C_HH_
