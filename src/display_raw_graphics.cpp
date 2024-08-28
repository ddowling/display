#include <Arduino.h>
#include "Pinout.h"
#include "DisplayLC7981.h"

DisplayLC7981 display;

const uint8_t pixel_w = 240;
const uint8_t pixel_h = 128;
const int frame_buffer_size = pixel_w * pixel_h / 8;
uint8_t frame_buffer[frame_buffer_size];

void clear()
{
    memset(frame_buffer, 0, frame_buffer_size);
}

void set_pixel(uint8_t x, uint8_t y)
{
    int b = (x / 8 + y * pixel_w/8);
    uint8_t m = 1<<(x%8);

    frame_buffer[b] |= m;
}

void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    int dx, sx;
    if (x0 < x1)
    {
        dx = x1 - x0;
        sx = 1;
    }
    else
    {
        dx = x0 - x1;
        sx = -1;
    }

    int dy, sy;
    if (y0 < y1)
    {
        dy = y0 - y1;
        sy = 1;
    }
    else
    {
        dy = y1 - y0;
        sy = -1;
    }
        
    int error = dx + dy;
    
    while(true)
    {
        set_pixel(x0, y0);
        
        if (x0 == x1 && y0 == y1)
            break;
        
        int e2 = 2 * error;
        if (e2 >= dy)
        {
            if (x0 == x1)
                break;
            
            error = error + dy;
            x0 = x0 + sx;
        }
        if (e2 <= dx)
        {
            if (y0 == y1)
                break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT); // LED on board
    pinMode(TRIGGER, OUTPUT); // Trigger pin on 3 pin header

    // Test graphics mode
    display.begin(pixel_w, pixel_h, true);
    display.setCursorAddress(0);
    display.fill(0, frame_buffer_size);
    
}

int pos_x = 0;
int pos_y = 0;
int vel_x = 1;
int vel_y = 2;

void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(TRIGGER, LOW);
    
    clear();
    set_pixel(10, 10);
    set_pixel(20, 20);
    set_pixel(100, 100);

    for (int r = 0; r < 7; r++)
        for(int c = 0; c < 7; c++)
            set_pixel(pos_x + c, pos_y + r);

    line(0, 0, 240, 128);
    line(0, 128, 240, 0);
    
    display.setCursorAddress(0);
    display.writeData(frame_buffer, frame_buffer_size);

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(TRIGGER, HIGH);

    pos_x += vel_x; 
    if (pos_x < 0)
    {
        pos_x = 0;
        vel_x = -vel_x;
    }
    else if (pos_x > pixel_w - 8)
    {
        vel_x = -vel_x;
    }
    
    pos_y += vel_y;
    if (pos_y < 0)
    {
        pos_y = 0;
        vel_y = -vel_y;
    }
    else if (pos_y > pixel_h - 8)
    {
        vel_y = -vel_y;
    }
}
