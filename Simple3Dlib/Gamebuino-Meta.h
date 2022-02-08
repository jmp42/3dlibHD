// foo

#ifndef __GAMEBUINO_META_H__
    #define __GAMEBUINO_META_H__
#include <QColor>
#include <stdint.h>
#include <qDebug>
//#include "modelViewer.hpp"

uint16_t rgb888Torgb565( uint8_t r, uint8_t g, uint8_t b ) {
   return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

QColor rgb565Torgb888( uint16_t u16_rgb565 ) {
    QColor col;
    col.setRed( (u16_rgb565>>8) & 0xF8 );
    col.setGreen( (u16_rgb565>>3) & 0xFC );
    col.setBlue( (u16_rgb565<<3) & 0xF8 );
   return col;
}


namespace Gamebuino_Meta {

    typedef uint16_t Color;
// Gamebuino_Meta::Color
}


void gb_end_update(  )
{

}

#define BUTTON_UP       1
#define BUTTON_DOWN     2
#define BUTTON_LEFT     4
#define BUTTON_RIGHT    8
#define BUTTON_A        16
#define BUTTON_B        32
class Display {
public:
    Gamebuino_Meta::Color vscreen[ SCREEN_WIDTH * SCREEN_HEIGHT ];

    void set_page( uint8_t u8_page_index ) {
        current_page_index = u8_page_index;
    }
/*
 * Natives functions not used if HD for max performances
    // gb.display.drawPixel(p->x,p->y,col);
    void drawPixel( int x, int y , Gamebuino_Meta::Color color ) {
        if ( y != last_y )
        if ( ( y < current_page_index*16 ) || ( y > current_page_index*16+15 ) )
        {
            qDebug() << " Draw outside " << y << " | " << current_page_index*16 << " to "  << current_page_index*SCREEN_BLOCK_HEIGHT+SCREEN_BLOCK_HEIGHT-1  ;
            last_y = y;
            return;
        }
        if ( ( x >= 0 ) && ( x < SCREEN_WIDTH ) && ( y >= 0 ) && ( y < SCREEN_HEIGHT ) )
            vscreen[ x + y * SCREEN_WIDTH ] = color;
    }
    */
    /*
     * Natives functions not used if HD for max performances
    // gb.display.clear();
    void clear(uint16_t u16_color) {
        for ( int y = current_page_index*16 ; y < current_page_index*SCREEN_BLOCK_HEIGHT+SCREEN_BLOCK_HEIGHT ; y++ )
            for ( int x = 0 ; x < SCREEN_WIDTH ; x++ )
                drawPixel( x, y, u16_color );
    };
    */
    uint8_t current_page_index;
    int last_y;
};

class Buttons {
public:
    //gb.buttons.timeHeld(BUTTON_DOWN)
    int timeHeld( uint32_t button_check )  {
        uint32_t u32_mask = u32_buttons & button_check;
        u32_buttons &= ~u32_mask;
        if ( u32_mask )
            return 1;
        return 0;
    }
    uint32_t u32_buttons;
};

class Gb {
public:
    Display display;
    Buttons buttons;
    //gb.begin();
    void begin() {  buttons.u32_buttons = 0; }
    //gb.setFrameRate(50);
    void setFrameRate(int) {}
    //gb.createColor(r,g,b);
    Gamebuino_Meta::Color createColor( int r, int g, int b ) { return rgb888Torgb565(r, g, b); }
    //gb.update();
    bool update() { return true; }
};



extern Gb gb;

#ifdef USE_INDEXED8_VSCREEN
uint8_t u8_buffer_draw[ SCREEN_WIDTH * SCREEN_BLOCK_HEIGHT ]; // 5 kB buffers
uint16_t u16_buffer_dma[ SCREEN_WIDTH * SCREEN_BLOCK_HEIGHT ]; // 10 kB buffers

uint8_t* gb_get_video_buffer( uint8_t u8_block_idex )
{
  return u8_buffer_draw;
}

#else

uint16_t u16_buffer0[ SCREEN_WIDTH * SCREEN_BLOCK_HEIGHT ]; // 5 kB buffers
uint16_t u16_buffer1[ SCREEN_WIDTH * SCREEN_BLOCK_HEIGHT ]; // 5 kB buffers

uint16_t* gb_get_video_buffer( uint8_t u8_block_idex )
{
  return (u8_block_idex&1)?u16_buffer1:u16_buffer0;
}

#endif

static inline uint16_t _swap16( uint16_t u16 )
{
  return (u16 << 8) |(u16 >> 8);
}

void gb_update( uint8_t u8_block_idex )
{
#ifdef USE_INDEXED8_VSCREEN

        // apply palette before DMA transfert
    extern const uint16_t* current_palette;
    for ( uint16_t u16_idx = 0 ; u16_idx < SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT ; u16_idx++ )
    {
        gb.display.vscreen[SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT*u8_block_idex + u16_idx] = (Gamebuino_Meta::Color)_swap16(current_palette[u8_buffer_draw[u16_idx]]);
    }

#else
    memcpy( &gb.display.vscreen[SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT*u8_block_idex], gb_get_video_buffer(u8_block_idex), SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT*2 );
#endif
//    Gamebuino_Meta::Color vscreen[ SCREEN_WIDTH * SCREEN_HEIGHT ];

}
#endif // of __GAMEBUINO_META_H__
