
#pragma once

// Used by the code to map G_D0 - G_D7 to GPIOA_BASE
#define FAST_IO 1

enum
{
    G_D0 = PA0,
    G_D1 = PA1,
    G_D2 = PA2,
    G_D3 = PA3,
    G_D4 = PA4,
    G_D5 = PA5,
    G_D6 = PA6,
    G_D7 = PA7,
    G_EN = PB10,
    G_CS = PB11,
    G_DC = PB0,
    G_RW = PB1,

    // PB3,PB4 are used by the SWD debug subsystem so cannot be used
    // when debugging.
    TRIGGER = PB5,
    
    BACKLIGHT = PB9,
    
};
    
