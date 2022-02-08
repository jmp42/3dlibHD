#ifndef __MINIMETA_H__
  #define __MINIMETA_H__

  // update screen bloc u8_block_idex
void gb_update( uint8_t u8_block_idex );
  // return the current video bloc buffer pointer ( used to draw the bloc frame )
#ifdef USE_INDEXED8_VSCREEN
    uint8_t* gb_get_video_buffer( uint8_t u8_block_idex );
#else
    uint16_t* gb_get_video_buffer( uint8_t u8_block_idex );
#endif
  // must be called before return from loop() to release DMA spi
void gb_end_update();

inline uint16_t swap16( uint16_t u16 )
{
  return (u16 << 8) |(u16 >> 8);
}



#ifndef WIN32
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
#endif // WIN32


#endif // of __MINIMETA_H__
