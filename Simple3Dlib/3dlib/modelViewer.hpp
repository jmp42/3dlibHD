/*
  Gamebuino META example demo for small3dlib -- model viewer.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#define USE_INDEXED8_VSCREEN

#ifndef __MODELVIEWVER_H__
    #define __MODELVIEWVER_H__


#define SCREEN_WIDTH        160
#define SCREEN_HEIGHT       128
#define SCREEN_FRAME_RATE  100
#ifdef USE_INDEXED8_VSCREEN             // use 5k draw buffer in indexed 8 bits, then 10k DMA buffer in 16 bits
    #define SCREEN_BLOCK_HEIGHT 32
#else
    #define SCREEN_BLOCK_HEIGHT 16
#endif

#include <Gamebuino-Meta.h>
#include "minimeta.h"

/* Before including small3dlib, we need to define some values for it, such as
  the resolution, the name of the pixel drawing function etc.: */

#define S3L_PIXEL_FUNCTION pixelFunc

#define S3L_RESOLUTION_X  SCREEN_WIDTH // 80..160
#define S3L_RESOLUTION_Y SCREEN_HEIGHT // 64..128
#define S3L_PAGE_Y       SCREEN_BLOCK_HEIGHT

#define S3L_Z_BUFFER 2   // this sets up a reduced precision z-buffer
#define S3L_SORT 0       // no sorting of triangles
#define S3L_STENCIL_BUFFER 0   // no stencil buffer
#define S3L_PERSPECTIVE_CORRECTION 0   /* perspective correction expensive and
                                          dosn't improve quality significantly
                                          with far away models, so turn it off,
                                          but you can try setting it it 2
                                          (approximation) */
#include "small3dlib.h"     // now we can include the library

// include the resources (converted using provided Python scripts):

#include "palette.h"
#include "houseModel.h"
#include "chestModel.h"
#include "earthModel.h"
#include "gbmetaModel.h"
#include "chestTexture_hd256.bmp.h"
#include "earth128x128.bmp.h"
#include "meta_256c.bmp.h"
#include "houseTexture_256c.bmp.h"
#include "logoprogrammez.bmp.h"
#include "logoprogrammezModel.h"

Gamebuino_Meta::Color paletteGB[256];

#define MIN_ZOOM (-6 * S3L_FRACTIONS_PER_UNIT)
#define MAX_ZOOM (-16 * S3L_FRACTIONS_PER_UNIT)

#define TEXTURE_W 128
#define TEXTURE_H 128

// helper global variabls and pointers:

int16_t previousTriangle = -1;
S3L_Vec4 uv0, uv1, uv2;
const uint8_t *texture = houseTexture_256c_data;
const S3L_Index *uvIndices = houseUVIndices;
const S3L_Unit *uvs = houseUVs;
const uint16_t* current_palette = houseTexture_256c_rgb565_palette;
uint8_t u8_texture_scale = 2;           // count of right shift to scale from 512 to texture size ( 1 = 256x256 tex, 2 = 128x128 )
static inline unsigned short sampleTexure(int32_t u, int32_t v)
{
  int index = v * TEXTURE_W + u;

  return texture[index];
}
#ifdef USE_INDEXED8_VSCREEN
    uint8_t* pu8_screen = 0;
#else
    uint16_t* pu16_screen = 0;
#endif

uint16_t current_page_index = 0;

/* This function will be called by the library to render individual pixels --
   remember, this is the bottleneck, it should be as fast as possible! */
inline void pixelFunc(S3L_PixelInfo *p)
{
  S3L_Unit u, v;   // texturing coordinates
  if ( ( p->y < current_page_index*SCREEN_BLOCK_HEIGHT ) || ( p->y > current_page_index*SCREEN_BLOCK_HEIGHT+SCREEN_BLOCK_HEIGHT-1 ) )
    return; // out of scrrent block

  if (p->triangleIndex != previousTriangle)
  {
    /* This is a per-triangle cache, it prevents computing per-triangle values
       for each pixel. We only recompute these when the triangleID changes. */

    S3L_getIndexedTriangleValues(p->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);
    /* ^ This is a helper funtion that retrieves the 3 UV coordinates of the
       triangle (one for each triangle vertex). */

    previousTriangle = p->triangleIndex;
  }
  
  u = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric)%S3L_FRACTIONS_PER_UNIT;
  v = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric)%S3L_FRACTIONS_PER_UNIT;

//  uint8_t c = sampleTexure(u >> u8_texture_scale,v >> u8_texture_scale);
  uint8_t c = sampleTexure(u >> 2,v >> 2);
              /* ^ Shifting by 3 gets the value from 0 to 512
                 (S3L_FRACTIONS_PER_UNIT) to 0 to 64 (texture resoltion). */

  // now write the pixel:
#ifdef USE_INDEXED8_VSCREEN
    pu8_screen[ p->x + (p->y - current_page_index*SCREEN_BLOCK_HEIGHT)*SCREEN_WIDTH ] = c;
#else
      Gamebuino_Meta::Color col = (Gamebuino_Meta::Color)swap16(current_palette[c]);
      pu16_screen[ p->x + (p->y - current_page_index*SCREEN_BLOCK_HEIGHT)*SCREEN_WIDTH ] = (uint16_t)col;
#endif
}

S3L_Scene scene; // our 3D scene, it will only hold one model at a time

void draw()
{
  S3L_newFrame();       // needs to be done before rendering a new frame
  S3L_transformScene(scene);
  for ( uint8_t u8_page_index = 0 ; u8_page_index < SCREEN_HEIGHT/SCREEN_BLOCK_HEIGHT ; u8_page_index++ )
  {
    
    S3L_zBufferClear();
#ifdef USE_INDEXED8_VSCREEN
    pu8_screen = gb_get_video_buffer(u8_page_index);
#else
    pu16_screen = gb_get_video_buffer(u8_page_index);
#endif
    current_page_index = u8_page_index;

#ifdef USE_INDEXED8_VSCREEN
    memset( pu8_screen, 0, SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT ); //   = gb.display.clear();
#else
    memset( pu16_screen, 0, SCREEN_WIDTH*SCREEN_BLOCK_HEIGHT*2 ); //   = gb.display.clear();
#endif

//    S3L_drawScene(scene, u8_page_index); // renders the 3D scene
    S3L_drawTransformedScene( scene, u8_page_index ); // renders the pre-transformed 3D scene
    if ( !u8_page_index ) 
    {
      gb_end_update();
      while ( !gb.update() );
    }
    
    gb_update(u8_page_index);
#ifndef WIN32
#ifndef USE_INDEXED8_VSCREEN
    ScreenRecorder::monitor( pu16_screen, u8_page_index); // <-- screen monitoring
#endif
#endif
  }

  
}

void setModel(uint8_t index)
{
#define modelCase(n)\
  scene.models = &(n##Model);\
  uvIndices = n##UVIndices;\
  uvs = n##UVs;             \
  current_palette = n##_rgb565_palette; \
  texture = n##_data;\
  u8_texture_scale = 2;

  switch (index)
  {
    case 1:
//      modelCase(chest)
      scene.models = &chestModel;
      uvIndices = chestUVIndices;
      uvs = chestUVs;
      current_palette = chestTexture_hd256_rgb565_palette;
      texture = chestTexture_hd256_data;
      u8_texture_scale = 2;
      break;
    
    case 2:
//      modelCase(earth)
      scene.models = &earthModel;
      uvIndices = earthUVIndices;
      uvs = earthUVs;
//      current_palette = earth_256c_rgb565_palette;
      current_palette = earth128x128_rgb565_palette;
//      texture = earth_256c_data;
      texture = earth128x128_data;
      u8_texture_scale = 2;
      break;

    case 3:
//      modelCase(gbmeta)
      scene.models = &gbmetaModel;
      uvIndices = gbmetaUVIndices;
      uvs = gbmetaUVs;
      current_palette = meta_256c_rgb565_palette;
      texture = meta_256c_data;
      u8_texture_scale = 2;
      break;

  case 4:
//      modelCase(cubetube)
//      modelCase(simple)
//      modelCase(cube)
      modelCase(logoprogrammez)
      /*
    scene.models = &simpleModel;
    uvIndices = simpleUVIndices;
    uvs = simpleUVs;
    current_palette = simple_rgb565_palette;
    texture = simple_data;
    u8_texture_scale = 2;
    break;
*/
    break;

      /*
  case 3:
//      modelCase(gbmeta)
    scene.models = &cubetubeModel;
    uvIndices = cubetubeUVIndices;
    uvs = cubetubeUVs;
    current_palette = cubetube_rgb565_palette;
    texture = cubetube_data;
    u8_texture_scale = 2;
    */
    break;

    default:
//      modelCase(house)
      scene.models = &houseModel;
      uvIndices = houseUVIndices;
      uvs = houseUVs;
      current_palette = houseTexture_256c_rgb565_palette;
      u8_texture_scale = 2;
      texture = houseTexture_256c_data;
      break;
  }

}

uint8_t modelIndex = 0;

void s3l_setup()
{

  for (uint16_t i = 0; i < 256; ++i)
  {
    uint8_t h,s,v,r,g,b;

    r = palette[i * 3];
    g = palette[i * 3 + 1];
    b = palette[i * 3 + 2];
    paletteGB[i] = (Gamebuino_Meta::Color)swap16((uint16_t)gb.createColor(r,g,b));
  }

  houseModelInit();
  chestModelInit();
  earthModelInit();
  gbmetaModelInit();
  logoprogrammezModelInit();

  S3L_initScene(&houseModel,1,&scene);

  setModel(0);

  scene.camera.transform.translation.z = -12 * S3L_FRACTIONS_PER_UNIT;
  // ^ place the camera a little bit to the front so that the model is seen

}

void s3l_loop()
{

  S3L_Unit rotationStep = 16;
  S3L_Unit zoomStep = 140;

  if (gb.buttons.timeHeld(BUTTON_A) > 0)
  {
    if (gb.buttons.timeHeld(BUTTON_DOWN) > 0)
      scene.camera.transform.translation.z =
        S3L_max(MAX_ZOOM,scene.camera.transform.translation.z - zoomStep);
    else if (gb.buttons.timeHeld(BUTTON_UP) > 0)
      scene.camera.transform.translation.z =
        S3L_min(MIN_ZOOM,scene.camera.transform.translation.z + zoomStep);
  }
  else
  {
    if (gb.buttons.timeHeld(BUTTON_UP) > 0)
      scene.models[0].transform.rotation.x += rotationStep;
    else if (gb.buttons.timeHeld(BUTTON_DOWN) > 0)
      scene.models[0].transform.rotation.x -= rotationStep;
        
    if (gb.buttons.timeHeld(BUTTON_RIGHT) > 0)
      scene.models[0].transform.rotation.y += rotationStep;
    else if (gb.buttons.timeHeld(BUTTON_LEFT) > 0)
      scene.models[0].transform.rotation.y -= rotationStep;
  }

  if (gb.buttons.timeHeld(BUTTON_B) == 1)
  {
    modelIndex = (modelIndex + 1) % 5;
    setModel(modelIndex);
  }

  draw();
}

#endif // of __MODELVIEWVER_H__

