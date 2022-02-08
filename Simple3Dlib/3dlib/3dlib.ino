#include <Gamebuino-Meta.h> 
#include <stdint.h>

#include "modelViewer.hpp" 

namespace Gamebuino_Meta {
    #define DMA_DESC_COUNT 3
    extern volatile uint32_t dma_desc_free_count; // get free dma dma 
    static SPISettings tftSPISettings = SPISettings(24000000, MSBFIRST, SPI_MODE0); // up to 58 fps
}
#ifdef USE_INDEXED8_VSCREEN
    uint8_t u8_buffer_draw[SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT]; // 5k static alloc
    uint16_t* u16_buffer_dma = 0;                               // 10k buffer located into initial scren allocation
#else
    uint16_t* u16_buffer0 = 0;
    uint16_t* u16_buffer1 = 0;
#endif

#ifdef USE_INDEXED8_VSCREEN
    uint8_t* gb_get_video_buffer( uint8_t u8_block_idex )
    {
        return u8_buffer_draw;
    }
#else
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
    if( 0 == u8_block_idex )
    {
        gb.tft.setAddrWindow(0, 0, SCREEN_WIDTH - 1 , SCREEN_HEIGHT - 1 );
        SPI.beginTransaction( Gamebuino_Meta::tftSPISettings );
        gb.tft.dataMode();
    }
    
    while ( Gamebuino_Meta::dma_desc_free_count < 3 );
#ifdef USE_INDEXED8_VSCREEN

  uint16_t u16_count = SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT;
  uint8_t* u8_psrc = u8_buffer_draw;
  uint16_t* u16_pdst = u16_buffer_dma;
  while ( u16_count-- )
    *u16_pdst++ = _swap16( current_palette[*u8_psrc++] );
    
    gb.tft.sendBuffer( u16_buffer_dma, SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT  ); 
  
#else
    gb.tft.sendBuffer( gb_get_video_buffer(u8_block_idex), SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT ); 
#endif
}

void gb_end_update(  )
{
    while ( Gamebuino_Meta::dma_desc_free_count < 3 );
    gb.tft.idleMode();
    SPI.endTransaction(); 
}


uint32_t u32_last = 0;
int i32_init_free, i32_postinit_free;
void setup() {
  gb.begin();
  i32_init_free = freeMemory();
    // Caution : gb.display.init(0, 0, ...) don't release to 80x64x16b RAM buffer => use for 2 VRAM bancks !!
//  transformed_vertex = (S3L_Vec4*) gb.display._buffer;
#ifdef USE_INDEXED8_VSCREEN
  u16_buffer_dma = gb.display._buffer;
  // u8_buffer_draw is allocated staticly
#else
  u16_buffer0 = gb.display._buffer;
  u16_buffer1 = gb.display._buffer;
  u16_buffer1 += 80*32; // half of initial 80x64x16b video buffer
#endif
  gb.display.init(0, 0, ColorMode::rgb565);
  gb.setFrameRate( SCREEN_FRAME_RATE );
  i32_postinit_free = freeMemory();
  SerialUSB.begin(9600);
  s3l_setup();
  
}




uint8_t u8_frame = 0;
void loop() {
 
  s3l_loop();
  
  u8_frame++;
  uint32_t u32_now = millis();
  if ( u32_now - u32_last > 1000 )
  {
      SerialUSB.print( 1000.0f*u8_frame /( u32_now-u32_last ) );
      SerialUSB.println(" fps" );
      u8_frame = 0;
      u32_last = u32_now;
      
      SerialUSB.print( " free ram " );
      SerialUSB.print( " init=" );
      SerialUSB.print( i32_init_free );
      SerialUSB.print( " post=" );
      SerialUSB.print( i32_postinit_free );
      
      SerialUSB.print( " curr=" );
      SerialUSB.println( freeMemory() );

  }

}

