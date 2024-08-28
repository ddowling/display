#include <Arduino.h>
#include "Pinout.h"
#include <U8g2lib.h>

// U8G2_LC7981_240X128_1_6800(rotation, d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc [, reset]) [page buffer, size = 240 bytes]
// U8G2_LC7981_240X128_2_6800(rotation, d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc [, reset]) [page buffer, size = 480 bytes]
// U8G2_LC7981_240X128_F_6800(rotation, d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc [, reset]) [full framebuffer, size = 3840 bytes]



U8G2_LC7981_240X128_F_6800 u8g2(U8G2_R0,
                                /* d0:d7*/ G_D0, G_D1, G_D2, G_D3, G_D4, G_D5, G_D6, G_D7,
                                /* enable */ G_EN,
                                /* cs */ G_CS,
                                /* dc */ G_DC
                                /* reset NC */);

void setup()
{
    Serial.begin(115200);
    Serial.println("Init start");

    pinMode(LED_BUILTIN, OUTPUT); // LED on board
    pinMode(TRIGGER, OUTPUT); // Trigger pin on 3 pin header
    pinMode(BACKLIGHT, OUTPUT);

    digitalWrite(BACKLIGHT, 1);

    // u8g2 does not control the RW line so always set to write
    pinMode(G_RW, OUTPUT);
    digitalWrite(G_RW, LOW);

    u8g2.begin();

    Serial.println("Init done");
}

int count = 0;
int delta_t_render = 0;
int delta_t_update = 0;

// draw something on the display with the `firstPage()`/nextPage()` loop
void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(TRIGGER, LOW);
    
    Serial.println("Loop");

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
