
#include "ui.h"
#include "logo.h"
#include "settings_eeprom.h"

namespace Ui {
    SSD1306 display;
    bool shouldDrawUpdate = false;
    bool shouldDisplay = false;
    bool shouldFullRedraw = false;


    void setup(I2C_HandleTypeDef *i2c_handle) {
        display.begin(i2c_handle, OLED_RESET, SSD1306_SWITCHCAPVCC, 0x3C);
        //display.begin(i2c_handle, OLED_RESET, SSD1306_SWITCHCAPVCC, 0x3D);

        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setTextWrap(false);
        display.setRotation(EepromSettings.screenFlip ? 2 : 0);
        display.clearDisplay();
    }

    void update() {
        if (shouldDisplay) {
        	if(display.display())
        		shouldDisplay = false;
        }
    }

    void drawLogo(){
    	display.drawBitmap(0, 0, logo,
    	SCREEN_WIDTH,
    	SCREEN_HEIGHT,
    	WHITE);
    	display.display();
    }

    void drawGraph(
        const uint8_t data[],
        const uint8_t dataSize,
        const uint8_t dataScale,
        const uint8_t x,
        const uint8_t y,
        const uint8_t w,
        const uint8_t h
    ) {
        #define SCALE_DATAPOINT(p) (p * h / dataScale)
        #define CLAMP_DATAPOINT(p) \
            (p > dataScale) ? dataScale : ((p < 0) ? 0 : p);

        Ui::clearRect(x, y, w - 1, h + 1);

        const uint8_t xScaler = w / (dataSize - 1);
        const uint8_t xScalarMissing = w - (xScaler * (dataSize - 1));

        uint8_t xNext = x;

        for (uint8_t i = 0; i < dataSize - 1; i++) {
            const uint8_t dataPoint = CLAMP_DATAPOINT(data[i]);
            const uint8_t dataPointNext = CLAMP_DATAPOINT(data[i + 1]);

            // Need to invert the heights so it shows the right way on the
            // screen.
            const uint8_t dataPointHeight = h - SCALE_DATAPOINT(dataPoint);
            const uint8_t dataPointNextHeight =
                h - SCALE_DATAPOINT(dataPointNext);

            const uint8_t xEnd = xNext + xScaler
                    + (i == 0 || i == dataSize - 2 ? (xScalarMissing + 1) / 2 : 0);

            Ui::display.drawLine(
                xNext,
                y + dataPointHeight,
                xEnd,
                y + dataPointNextHeight,
                WHITE
            );

            xNext = xEnd;
        }

        #undef SCALE_DATAPOINT
        #undef CLAMP_DATAPOINT
    }


    void drawDashedHLine(
        const int x,
        const int y,
        const int w,
        const int step
    ) {
        for (int i = 0; i <= w; i += step) {
            Ui::display.drawFastHLine(x + i, y, step / 2, WHITE);
        }
    }

    void drawDashedVLine(
        const int x,
        const int y,
        const int h,
        const int step
    ) {
        for (int i = 0; i <= h; i += step) {
            Ui::display.drawFastVLine(x, y + i, step / 2, INVERSE);
        }
    }

    void clear() {
        display.clearDisplay();
    }

    void clearRect(const int x, const int y, const int w, const int h) {
        display.fillRect(x, y, w, h, BLACK);
    }


    void needUpdate() {
        shouldDrawUpdate = true;
    }

    void needDisplay() {
        shouldDisplay = true;
    }

    void needFullRedraw() {
        shouldFullRedraw = true;
    }
}
