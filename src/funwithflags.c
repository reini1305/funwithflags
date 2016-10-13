#include <pebble.h>
#include "mapping_resource.h"
#include "mapping_names.h"
//#include <nightstand/nightstand.h>


#define NUM_COUNTRIES 256
#ifdef PBL_ROUND
#define OFFSET_TOP 5
#define OFFSET_BOTTOM 55
#else
#define OFFSET_TOP 0
#define OFFSET_BOTTOM 40
#endif

static Window *window;
static TextLayer *time_layer;
static TextLayer *text_layer;
static BitmapLayer* flags_layer;
static GBitmap* flag_bitmap;
static int country_id;
static AppTimer* show_name_timer=NULL;
static char time_text[] = "00:00";

void unobstructed_change(AnimationProgress progress, void* data) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
  // update layer positions
  layer_set_frame(text_layer_get_layer(text_layer),GRect(0, bounds.size.h-OFFSET_BOTTOM, bounds.size.w, 40));
  layer_set_frame(text_layer_get_layer(time_layer),GRect(0, bounds.size.h-OFFSET_BOTTOM, bounds.size.w, 40));
  layer_set_frame(bitmap_layer_get_layer(flags_layer),GRect(0,OFFSET_TOP,bounds.size.w,bounds.size.h-OFFSET_BOTTOM+OFFSET_TOP));
}

static void updateFlag() {
	country_id = rand()%NUM_COUNTRIES;//(country_id+1)%NUM_COUNTRIES;
	if(flag_bitmap)
		gbitmap_destroy(flag_bitmap);
	flag_bitmap = gbitmap_create_with_resource(resource_names[country_id]);
	bitmap_layer_set_bitmap(flags_layer,flag_bitmap);
	text_layer_set_text(text_layer, country_names[country_id]);
}

void hide_name(void* data) {
  layer_set_hidden(text_layer_get_layer(text_layer),true);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {

  layer_set_hidden(text_layer_get_layer(text_layer),false);
  if(!app_timer_reschedule(show_name_timer,2000))
    show_name_timer = app_timer_register(2000,hide_name,NULL);
}

static void tickHandler(struct tm *tick_time, TimeUnits units) {

	char *time_format;
	if (clock_is_24h_style()) {
	    time_format = "%R";
	} else {
	    time_format = "%I:%M";
	}

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  //if(!nightstand_window_update()) {
    updateFlag();
    layer_mark_dirty(text_layer_get_layer(time_layer));
    layer_set_hidden(text_layer_get_layer(time_layer),false);
    layer_set_hidden(bitmap_layer_get_layer(flags_layer),false);
  //}
}

static void loadWindow(Window *window) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(window));
  window_set_background_color(window,GColorBlack);

  time_layer = text_layer_create(GRect(0, bounds.size.h-OFFSET_BOTTOM, bounds.size.w, 40));
  text_layer_set_background_color(time_layer, GColorBlack);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text(time_layer,time_text);

  text_layer = text_layer_create(GRect(0, bounds.size.h-OFFSET_BOTTOM, bounds.size.w, 40));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text(text_layer,"bla");

  flags_layer = bitmap_layer_create(GRect(0,OFFSET_TOP,bounds.size.w,bounds.size.h-OFFSET_BOTTOM+OFFSET_TOP));
  bitmap_layer_set_alignment(flags_layer,GAlignCenter);
  updateFlag();

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(flags_layer));


  layer_set_hidden(text_layer_get_layer(text_layer),true);
  layer_set_hidden(text_layer_get_layer(time_layer),true);
  layer_set_hidden(bitmap_layer_get_layer(flags_layer),true);

  accel_tap_service_subscribe(&accel_tap_handler);

}

static void unloadWindow(Window *window) {

}


static void init() {
  srand(time(NULL));
  flag_bitmap=NULL;
  country_id = rand()%NUM_COUNTRIES;
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
      .load = loadWindow,
      .unload = unloadWindow
  });

  UnobstructedAreaHandlers handlers = {
    // .will_change = unobstructed_will_change,
    .change = unobstructed_change
    // .did_change = unobstructed_did_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);
  window_stack_push(window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tickHandler);
  //nightstand_window_init();
}

static void deinit() {
  window_destroy(window);
  //nightstand_window_deinit();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
