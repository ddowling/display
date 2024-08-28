#include <Arduino.h>
#include "Pinout.h"
#include "DisplayLC7981.h"

#define DEBUG_DISPLAY 0
#define POLL_BUSY 0

static inline void delay_ns(uint32_t ns)
{
    uint32_t c = ns * (1000000000/SystemCoreClock);

    while(c-- > 0)
    {
        asm("nop");
    }
}

void DisplayLC7981::begin(uint8_t h_pixels, uint8_t v_pixels, bool graphic_mode)
{
    pinMode(G_RW, OUTPUT);
    pinMode(G_EN, OUTPUT);
    pinMode(G_CS, OUTPUT);
    pinMode(G_DC, OUTPUT);
    
    digitalWrite(G_EN, LOW);
    digitalWrite(G_DC, HIGH);
    digitalWrite(G_CS, HIGH);
    busDirection(OUTPUT);

    uint8_t mode;
    uint8_t vp; /* 1 to 16 */
    uint8_t hp; /* 6 to 8 */
    if (graphic_mode)
    {
        mode = 0x32;
        vp = 1;
        hp = 8;
    }
    else
    {
        mode = 0x3c;
        vp = 8;
        hp = 6;
    }
    writeRegister(MODE_CONTROL, mode);
#if POLL_BUSY
    busy(true);
#endif

#if DEBUG_DISPLAY
    Serial.printf("graphic_mode=%d MODE_CONTROL=0x%02x\r\n", graphic_mode, mode);
#endif
    
    uint8_t v = ((vp-1)<<4) | (hp-1);
    writeRegister(CHARACTER_PITCH, v);
#if POLL_BUSY
    busy(true);
#endif
    
#if DEBUG_DISPLAY
    Serial.printf("vp=%d hp=%d CHARACTER_PITCH=0x%02x\r\n", vp, hp, v);
#endif

    v = h_pixels/hp - 1;
    writeRegister(NUM_CHARACTERS, v);
#if POLL_BUSY
    busy(true);
#endif

#if DEBUG_DISPLAY
    Serial.printf("h_pixels=%d NUM_CHARACTER=0x%02x\r\n", h_pixels, v);
#endif
    
    v = v_pixels - 1;
    writeRegister(TIME_DIVISION, v);
#if POLL_BUSY
    busy(true);
#endif

#if DEBUG_DISPLAY
    Serial.printf("v_pixels=%d TIME_DIVISION=0x%02x\r\n", v_pixels, v);
#endif
    
    writeRegister(DISPLAY_ADDRESS_L, 0);
#if POLL_BUSY
    busy(true);
#endif
    
    writeRegister(DISPLAY_ADDRESS_H, 0);
#if POLL_BUSY
    busy(true);
#endif

    setCursorAddress(0);
}

void DisplayLC7981::busDirection(uint8_t dir)
{
    pinMode(G_D0, dir);
    pinMode(G_D1, dir);
    pinMode(G_D2, dir);
    pinMode(G_D3, dir);
    pinMode(G_D4, dir);
    pinMode(G_D5, dir);
    pinMode(G_D6, dir);
    pinMode(G_D7, dir);

    digitalWrite(G_RW, (dir == INPUT));
}

void DisplayLC7981::setTarget(Target t)
{
    digitalWrite(G_DC, t);
}

void DisplayLC7981::busWrite(uint8_t data)
{
#if FAST_IO
    // Faster to clear the bits and set them than using read modify write
    //GPIOA->ODR = (GPIOA->ODR & ~0xff) | data;
    GPIOA->BSRR = data | (0xff<<16);
#else
    digitalWrite(G_D0, (data & 0x01) != 0);
    digitalWrite(G_D1, (data & 0x02) != 0);
    digitalWrite(G_D2, (data & 0x04) != 0);
    digitalWrite(G_D3, (data & 0x08) != 0);
    digitalWrite(G_D4, (data & 0x10) != 0);
    digitalWrite(G_D5, (data & 0x20) != 0);
    digitalWrite(G_D6, (data & 0x40) != 0);
    digitalWrite(G_D7, (data & 0x80) != 0);
#endif

    // FIXME What should this delay be? 1us works well!
    //delayMicroseconds(1);
    // FIXME Guessing here!!
    delay_ns(200);
    
    enable(HIGH);

    delay_ns(200);
    
    enable(LOW);
}

uint8_t DisplayLC7981::busRead()
{
    enable(HIGH);

#if FAST_IO
    uint8_t result = GPIOA->IDR & 0xff;
#else
    uint8_t result = 0;
    if (digitalRead(G_D0))
        result |= 0x01;
    if (digitalRead(G_D1))
        result |= 0x02;
    if (digitalRead(G_D2))
        result |= 0x04;
    if (digitalRead(G_D3))
        result |= 0x08;
    if (digitalRead(G_D4))
        result |= 0x10;
    if (digitalRead(G_D5))
        result |= 0x20;
    if (digitalRead(G_D6))
        result |= 0x40;
    if (digitalRead(G_D7))
        result |= 0x80;
#endif

    enable(LOW);

    return result;
}

void DisplayLC7981::enable(bool b)
{
#if false && FAST_IO
    if (b)
        GPIOB->BSRR = 1<<10;
    else
        GPIOB->BRR = 1<<10;
#else
    digitalWrite(G_EN, b);
#endif
}

void DisplayLC7981::writeRegister(Register reg, uint8_t val)
{
    digitalWrite(G_CS, LOW);

    setTarget(REGISTER);
    busWrite(reg);
    setTarget(DATA);
    busWrite(val);

    digitalWrite(G_CS, HIGH);
}

void DisplayLC7981::setCursorAddress(uint16_t address)
{
    writeRegister(CURSOR_ADDRESS_L, address & 0xff);
    writeRegister(CURSOR_ADDRESS_H, address>>8);
    needDummyRead = true;
}
    
void DisplayLC7981::writeData(const uint8_t *buf, uint16_t len)
{
    digitalWrite(G_CS, LOW);

    setTarget(REGISTER);
    busWrite(WRITE_DATA);

    setTarget(DATA);
    for (uint16_t i = 0; i < len; i++)
        busWrite(buf[i]);

    digitalWrite(G_CS, HIGH);
}

static uint8_t flipByte(uint8_t c)
{
    uint8_t r = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        r <<= 1;
        r |= c & 1;
        c >>= 1;
    }
    
    return r;
}

void DisplayLC7981::writeDataFlipped(const uint8_t *buf, uint16_t len)
{
    digitalWrite(G_CS, LOW);

    setTarget(REGISTER);
    busWrite(WRITE_DATA);

    setTarget(DATA);
    for (uint16_t i = 0; i < len; i++)
        busWrite(flipByte(buf[i]));

    digitalWrite(G_CS, HIGH);
}

void DisplayLC7981::writeString(const char *s)
{
    writeData((const uint8_t *)s, strlen(s));
}

void DisplayLC7981::fill(uint8_t c, uint16_t len)
{
    digitalWrite(G_CS, LOW);

    setTarget(REGISTER);
    busWrite(WRITE_DATA);

    setTarget(DATA);
    for (uint16_t i = 0; i < len; i++)
        busWrite(c);

    digitalWrite(G_CS, HIGH);
}

void DisplayLC7981::readData(uint8_t *buf, uint16_t len)
{
    digitalWrite(G_CS, LOW);

    setTarget(REGISTER);
    busWrite(READ_DATA);

    setTarget(DATA);
    busDirection(INPUT);

    // dummy read if cursor changed
    if (needDummyRead)
    {
        busRead();
        needDummyRead = false;
    }
    
    for (uint16_t i = 0; i < len; i++)
        buf[i] = busRead();

    busDirection(OUTPUT);
    digitalWrite(G_CS, HIGH);
}

bool DisplayLC7981::busy(bool wait)
{
    digitalWrite(G_CS, LOW);
    setTarget(REGISTER);
    busDirection(INPUT);

    uint8_t b;
    while(true)
    {
        b = busRead();

#if DEBUG_DISPLAY
        Serial.printf("Busy read %x\r\n", b);
#endif

        if ((b & 0x80) == 0 || !wait)
            break;
    }
    
    busDirection(OUTPUT);
    digitalWrite(G_CS, HIGH);

    return (b & 0x80) != 0;
}
