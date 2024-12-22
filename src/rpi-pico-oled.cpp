/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers
 **************************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define console Serial
#define uart Serial1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire /*i2c*/,
			 -1 /*reset-pin*/);

void setup()
{
	/* hardware uart */
	uart.setTX(0);
	uart.setRX(1);
	uart.begin(9600);

	Wire.setSDA(4);
	Wire.setSCL(5);
	Wire.begin();

	uart.println(F("Initializing SSD1306..."));

	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	display.begin(SSD1306_SWITCHCAPVCC,0x3c /* i2c address, 7bits */);

	/* will display logo */
	display.display();
	delay(500);

	/* will display hello world. */
	display.clearDisplay();
	display.setTextSize(1); // Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE); // Draw white text
	display.cp437(true); // Use full 256 char 'Code Page 437' font

	display.setCursor(0, 0); // Start at top-left corner
	//                 --------------------
	display.println(F("ssd1306 OLED display"));

	display.setCursor(0, 8);
	display.println(F("/dev/ttyACMx ^B help"));

	display.setCursor(0, 16);
	display.println(F("github.com/vogelchr/"));

	display.setCursor(0, 24);
	display.println(F("rpi-pico-oled"));

	display.drawRect(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, 5, 5,
			 SSD1306_WHITE);
	display.display();
}

unsigned char fixme_will_be_overwritten[1024];
unsigned char *fixme_p = fixme_will_be_overwritten;
unsigned char *fixme_end = fixme_p + sizeof(fixme_will_be_overwritten);

static unsigned char linebuf[21]; /* max 31 characters/line */
static unsigned int row=0;
static unsigned int col=0;


void drawCursor(unsigned int col, unsigned int row, unsigned int color) {
	int x = col * 6;
	int y = row * 8;
	int w = MIN(6,SCREEN_WIDTH-x-1);

	display.fillRect(x,y,w,8,color);
}

void loop()
{
	unsigned char *p;
	unsigned int j;
	int chr;
	
	*fixme_p++ = 0;
	if (fixme_p == fixme_end)
		fixme_p = fixme_will_be_overwritten;

	chr = console.read();
	if (chr == -1)
		return;

	switch (chr) {
	case '\r': // Carriage Return
		drawCursor(col, row, SSD1306_BLACK);
		col=0;
		drawCursor(col, row, SSD1306_WHITE);
		display.display();
		break;
	case '\n': // Linefeed
		drawCursor(col, row, SSD1306_BLACK);
		row++;
		if (row > 3)
			row = 0;
		col=0;
		drawCursor(col, row, SSD1306_WHITE);
		display.display();
		break;
	case '\2': // Ctrl-B
		console.println(F("Ctrl-B: this help"));
		console.println(F("Ctrl-A: row/col info"));
		console.println(F("Ctrl-L: clear"));
		break;
	case '\14': // Ctrl-L
		display.clearDisplay();
		col=0;
		row=0;
		drawCursor(col, row, SSD1306_WHITE);
		display.display();
		break;
	case '\1': // Ctrl-A
		console.print(F("row="));
		console.println(row);
		console.print(F("col="));
		console.println(col);
		break;
	default:
		display.setCursor(col*6, row*8); // Start at top-left corner
		drawCursor(col, row, SSD1306_BLACK);
		display.write(chr);
		if (col < 20) {
			col++;
		}
		drawCursor(col, row, SSD1306_WHITE);
		display.display();
	}
}
