#pragma once
#include <pebble.h>  

// used to pass bimap info to get/set pixel accurately  
typedef struct {
   GBitmap *bitmap;  // actual bitmap for Chalk raw manipulation
   uint8_t *bitmap_data;
   int bytes_per_row;
   GBitmapFormat bitmap_format;
}  BitmapInfo;
  
typedef void effect_cb(GContext* ctx, GRect position, void* param);

// Rotate 90 degrees
// Added by Ron64
// Parameter: true: rotate right/clockwise false: rotate left/counter_clockwise
effect_cb effect_rotate_90_degrees;

