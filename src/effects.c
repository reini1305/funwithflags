#include <pebble.h>
#include "effects.h"
//#include "math.h"
  
  
// { ********* Graphics utility functions (probablu should be seaparated into anothe file?) *********
  
  
// set pixel color at given coordinates 
void set_pixel(BitmapInfo bitmap_info, int y, int x, uint8_t color) {
  
#ifndef PBL_PLATFORM_APLITE  
  if (bitmap_info.bitmap_format == GBitmapFormat1BitPalette) { // for 1bit palette bitmap on Basalt --- verify if it needs to be different
     bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x / 8] ^= (-color ^ bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x / 8]) & (1 << (x % 8)); 
#else
  if (bitmap_info.bitmap_format == GBitmapFormat1Bit) { // for 1 bit bitmap on Aplite  --- verify if it needs to be different
     bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x / 8] ^= (-color ^ bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x / 8]) & (1 << (x % 8)); 
#endif
  } else { // othersise (assuming GBitmapFormat8Bit) going byte-wise
      
     #ifndef PBL_PLATFORM_CHALK
       bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x] = color;
     #else
       GBitmapDataRowInfo info = gbitmap_get_data_row_info(bitmap_info.bitmap, y);
       if ((x >= info.min_x) && (x <= info.max_x)) info.data[x] = color;
     #endif  
  
  }
      
}

// get pixel color at given coordinates 
uint8_t get_pixel(BitmapInfo bitmap_info, int y, int x) {

#ifndef PBL_PLATFORM_APLITE  
  if (bitmap_info.bitmap_format == GBitmapFormat1BitPalette) { // for 1bit palette bitmap on Basalt shifting left to get correct bit
    return (bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x / 8] << (x % 8)) & 128;
#else
  if (bitmap_info.bitmap_format == GBitmapFormat1Bit) { // for 1 bit bitmap on Aplite - shifting right to get bit
    return (bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x / 8] >> (x % 8)) & 1;
#endif
  } else {  // othersise (assuming GBitmapFormat8Bit) going byte-wise
    
    #ifndef PBL_PLATFORM_CHALK
       return bitmap_info.bitmap_data[y*bitmap_info.bytes_per_row + x]; 
     #else
       GBitmapDataRowInfo info = gbitmap_get_data_row_info(bitmap_info.bitmap, y);
       if ((x >= info.min_x) && (x <= info.max_x))
         return info.data[x];
       else 
         return -1;
     #endif  
  }
  
}  

//  ********* Graphics utility functions (probablu should be seaparated into anothe file?) ********* }

 
// Rotate 90 degrees
// Added by Ron64
// Parameter:  true: rotate right/clockwise,  false: rotate left/counter_clockwise
void effect_rotate_90_degrees(GContext* ctx,  GRect position, void* param){

  //capturing framebuffer bitmap
  GBitmap *fb = graphics_capture_frame_buffer(ctx);
  
  BitmapInfo bitmap_info;
  bitmap_info.bitmap = fb;
  bitmap_info.bitmap_data =  gbitmap_get_data(fb);
  bitmap_info.bytes_per_row = gbitmap_get_bytes_per_row(fb);
  bitmap_info.bitmap_format = gbitmap_get_format(fb);
  
  bool right = (bool)param;
  uint8_t qtr, xCn, yCn, temp_pixel;
  xCn= position.origin.x + position.size.w /2;
  yCn= position.origin.y + position.size.h /2;
  qtr=position.size.w;
  if (position.size.h < qtr)
    qtr= position.size.h;
  qtr= qtr/2;

  for (int c1 = 0; c1 < qtr; c1++)
    for (int c2 = 1; c2 < qtr; c2++){
      temp_pixel = get_pixel(bitmap_info, yCn +c1, xCn +c2);
      if (right){
        set_pixel(bitmap_info, yCn +c1, xCn +c2, get_pixel(bitmap_info, yCn -c2, xCn +c1));
        set_pixel(bitmap_info, yCn -c2, xCn +c1, get_pixel(bitmap_info, yCn -c1, xCn -c2));
        set_pixel(bitmap_info, yCn -c1, xCn -c2, get_pixel(bitmap_info, yCn +c2, xCn -c1));
        set_pixel(bitmap_info, yCn +c2, xCn -c1, temp_pixel);
      }
      else{
        set_pixel(bitmap_info, yCn +c1, xCn +c2, get_pixel(bitmap_info, yCn +c2, xCn -c1));
        set_pixel(bitmap_info, yCn +c2, xCn -c1, get_pixel(bitmap_info, yCn -c1, xCn -c2));
        set_pixel(bitmap_info, yCn -c1, xCn -c2, get_pixel(bitmap_info, yCn -c2, xCn +c1));
        set_pixel(bitmap_info, yCn -c2, xCn +c1, temp_pixel);
      }
     }
  
  graphics_release_frame_buffer(ctx, fb);
}

