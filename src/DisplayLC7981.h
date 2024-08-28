#pragma once

class DisplayLC7981
{
public:
    void begin(uint8_t h_pixels=240,
               uint8_t v_pixels=128,
               bool graphic_mode=true);

    // High level API
    void setCursorAddress(uint16_t address);
    void writeData(const uint8_t *buf, uint16_t len);
    void writeDataFlipped(const uint8_t *buf, uint16_t len);
    
    void writeString(const char *s);
    void fill(uint8_t c, uint16_t len);
    void readData(uint8_t *buf, uint16_t len);
    
    enum Target
    {
        REGISTER = 1,
        DATA = 0
    };
    
    // Low level register access
    enum Register
    {
        MODE_CONTROL = 0x00,
        CHARACTER_PITCH = 0x01,
        NUM_CHARACTERS = 0x02,
        TIME_DIVISION = 0x03,
        CURSOR_POSITION = 0x04,
        DISPLAY_ADDRESS_L = 0x08,
        DISPLAY_ADDRESS_H = 0x09,
        CURSOR_ADDRESS_L = 0x0a,
        CURSOR_ADDRESS_H = 0x0b,
        WRITE_DATA = 0x0c,
        READ_DATA = 0x0d,
        BIT_CLEAR = 0x0e,
        BIT_SET = 0x0f
    };

    void writeRegister(Register reg, uint8_t val);

    // Poll the BUSY flag to determine if the display is busy.
    // Use wait=true to wait until the display is not busy
    bool busy(bool wait);

protected:
    bool needDummyRead;

    // Set to either OUTPUT or INPUT
    void busDirection(uint8_t dir);

    void setTarget(Target t);
    void busWrite(uint8_t data);
    uint8_t busRead();

    void enable(bool b);
};
