#include <Arduino.h>
#include "Pinout.h"
#include "DisplayLC7981U8G2.h"

DisplayLC7981U8G2 u8g2;

void setup()
{
    Serial.begin(115200);
    Serial.printf("glue_test start\r\n");

    pinMode(LED_BUILTIN, OUTPUT); // LED on board

    pinMode(TRIGGER, OUTPUT); // Trigger pin on 3 pin header

    pinMode(BACKLIGHT, OUTPUT);
    digitalWrite(BACKLIGHT, 1);

    Serial.printf("About to do display.begin()\r\n");
    Serial.flush();

    u8g2.begin();

    Serial.printf("Init done\r\n");
}

int count = 0;
int delta_t_render = 0;
int delta_t_update = 0;

// draw something on the display with the `firstPage()`/nextPage()` loop
void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(TRIGGER, LOW);
    
    Serial.printf("Loop\r\n");

    unsigned long start = millis();

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "Hello World!");

    char buf[80];
    sprintf(buf, "Count = %d", count);
    u8g2.drawStr(0, 100, buf);

    u8g2.setFont(u8g2_font_profont12_mf);
    sprintf(buf, "delta_t render = %dms update = %dms",
            delta_t_render, delta_t_update);
    u8g2.drawStr(0, 124, buf);

    unsigned long end_render = millis();
    
    u8g2.updateDisplay();

    unsigned long end_update = millis();
    delta_t_render = end_render - start;
    delta_t_update = end_update - end_render;
    
    count++;
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(TRIGGER, HIGH);

//    delay(100);
}
