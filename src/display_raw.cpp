#include <Arduino.h>
#include "Pinout.h"
#include "DisplayLC7981.h"

DisplayLC7981 display;

void setup()
{
    Serial.begin(115200);
    Serial.println("Init start");

    pinMode(LED_BUILTIN, OUTPUT); // LED on board
    pinMode(TRIGGER, OUTPUT); // Trigger pin on 3 pin header

    // Test text mode
    display.begin(240, 128, false);

    display.setCursorAddress(0);
    display.fill(0x20, 40*16);

    display.setCursorAddress(0);
    display.writeString("Hello World123456789012345678901234567890");
    display.writeString("This is another string");
    for (uint8_t i = 0; i < 100; i++)
    {
        char buf[10];
        sprintf(buf, "Blah%i", i);
        display.writeString(buf);
    }
    
    display.setCursorAddress(0);
    for (int i = 0; i < 100; i++)
    {
        uint8_t v;
        display.readData(&v, sizeof(v));
        Serial.printf("[%d] = %d\r\n", i, v);
    }
    Serial.println("Init done");
}

void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(TRIGGER, LOW);
    
    Serial.println("Loop");

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(TRIGGER, HIGH);

    delay(1000);
}
