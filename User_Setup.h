#define USER_SETUP_LOADED

#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_BL   21
#define TFT_BACKLIGHT_ON HIGH

// Pin SPI Hardware condivisi
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4

// Assegnazione del pin Touch Chip Select per Freenove
#define TOUCH_CS  25 

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SPI_FREQUENCY          27000000
#define SPI_READ_FREQUENCY     20000000
// QUESTA È LA RIGA CRUCIALE: imposta la frequenza corretta richiesta dal chip del touch Freenove
#define SPI_TOUCH_FREQUENCY     25000000 
