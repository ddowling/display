#include "DisplayLC7981U8G2.h"
#include "DisplayLC7981.h"

#define DEBUG_CB 0

// Low level raw display
// FIXME pass in reference
static DisplayLC7981 display_lc7981;

static const u8x8_display_info_t u8x8_lc7981_240x128_display_info =
{
  /* chip_enable_level = */ 0,	/* LC7981 has a low active CS*/
  /* chip_disable_level = */ 1,
  
  /* from here... */
  /* post_chip_enable_wait_ns = */ 20,	
  /* pre_chip_disable_wait_ns = */ 20,	
  /* reset_pulse_width_ms = */ 1, 	
  /* post_reset_wait_ms = */ 10, 	
  /* sda_setup_time_ns = */ 30,		
  /* sck_pulse_width_ns = */ 65,	/* half of cycle time  */
  /* sck_clock_hz = */ 4000000UL,	/* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
  /* spi_mode = */ 0,		/* active high, rising edge */
  /* i2c_bus_clock_100kHz = */ 4,
  /* ... to here, values are ignored, because this is a parallel interface only */
  
  /* data_setup_time_ns = */ 220,	
  /* write_pulse_width_ns = */ 20,	
  /* tile_width = */ 30,		/* width of 30*8=240 pixel */
  /* tile_height = */ 16,
  /* default_x_offset = */ 0,	
  /* flipmode_x_offset = */ 0,	
  /* pixel_width = */ 240,
  /* pixel_height = */ 128
};

static uint8_t display_cb(u8x8_t *u8x8,
                          uint8_t msg,
                          uint8_t arg_int,
                          void *arg_ptr)
{
    if (msg == U8X8_MSG_DISPLAY_SETUP_MEMORY)
    {
	u8x8_d_helper_display_setup_memory(u8x8,
                                           &u8x8_lc7981_240x128_display_info);
        return 1;
    }
    else if (msg == U8X8_MSG_DISPLAY_INIT)
    {
        display_lc7981.begin();
        return 1;
    }
    else if (msg == U8X8_MSG_DISPLAY_DRAW_TILE)
    {
        uint8_t i, j;

        u8x8_tile_t *tile = (u8x8_tile_t *)arg_ptr;
        
        uint16_t y = tile->y_pos;
        y *= 8;
        y *= u8x8->display_info->tile_width;
        /* x = tile->x_pos; x is ignored... no u8x8 support */

        // FIXME Support Start  and end transfer
        //u8x8_cad_StartTransfer(u8x8);
        uint8_t c = tile->cnt;	/* number of tiles */
        uint8_t *ptr = tile->tile_ptr;	/* data ptr to the tiles */
        for( i = 0; i < 8; i++ )
        {
            display_lc7981.setCursorAddress(y);

            // U8G2 uses a different byte order.
            // FIXME Can some of the display_info_t properties swap this?
            display_lc7981.writeDataFlipped(ptr, c);
            ptr += c;
	
            y += u8x8->display_info->tile_width;
        }

        //u8x8_cad_EndTransfer(u8x8);
        return 1;
    }
    else
        return 1;
}

static uint8_t cad_cb(u8x8_t *u8x8,
                       uint8_t msg,
                       uint8_t arg_int,
                       void *arg_ptr)
{
#if DEBUG_CB
    Serial.printf("cad_cb(msg=%d arg_int=%d arg_ptr=%p)\r\n",
                  msg, arg_int, arg_ptr);
    Serial.flush();
#endif

    return 1;
}

static uint8_t byte_cb(u8x8_t *u8x8,
                       uint8_t msg,
                       uint8_t arg_int,
                       void *arg_ptr)
{
#if DEBUG_CB
    Serial.printf("byte_cb(msg=%d arg_int=%d arg_ptr=%p)\r\n",
                  msg, arg_int, arg_ptr);
    Serial.flush();
#endif
    
    return 1;
}

static uint8_t gpio_and_delay_cb(u8x8_t *u8x8,
                                 uint8_t msg,
                                 uint8_t arg_int,
                                 void * /*arg_ptr*/)
{
#if DEBUG_CB
    Serial.printf("gpio_and_delay_cb(msg=%d arg_int=%d)\r\n",
                  msg, arg_int);
    Serial.flush();
#endif

    return 1;
}

DisplayLC7981U8G2::DisplayLC7981U8G2()
{
    const u8g2_cb_t *rotation = U8G2_R0;

    // FIXME Replace all unused cad_cb etc callbacks with u8x8_dummy_cb
    u8g2_SetupDisplay(&u8g2,
                      display_cb,
                      cad_cb,
                      byte_cb,
                      gpio_and_delay_cb);
    
    uint8_t tile_buf_height;
#if 1
    uint8_t *buf = u8g2_m_30_16_f(&tile_buf_height);
#else
    static uint8_t buf[3840];
    tile_buf_height = 16;
#endif

#if DEBUG_CB
    Serial.printf("u8g2_SetupBuffer th=%d\r\n", tile_buf_height);
    Serial.flush();
#endif
    
    u8g2_SetupBuffer(&u8g2,
                     buf,
                     tile_buf_height,
                     u8g2_ll_hvline_horizontal_right_lsb,
                     rotation);

#if DEBUG_CB
    Serial.printf("Constructor done\r\n");
    Serial.flush();
#endif
}
