#include <pebble.h>

#include "localize.h"
#include "enamel.h"

static Window *window;
static TextLayer *text_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, _("Select"));
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, _("Up"));
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, _("Down"));
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(text_layer, _("Press a button"));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static uint32_t get_resource_locale(LANGUAGEValue language){
  switch(language){
    case LANGUAGE_FRENCH  : return RESOURCE_ID_LOCALE_FRENCH;
    case LANGUAGE_MARTIAN : return RESOURCE_ID_LOCALE_MARTIAN;
    default :               return RESOURCE_ID_LOCALE_DEFAULT;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  locale_init(get_resource_locale(enamel_get_language()));
  text_layer_set_text(text_layer, _("Press a button"));
}

static void init(void) {
  enamel_init(0, 0);
  enamel_register_custom_inbox_received(in_received_handler);

  locale_init(get_resource_locale(enamel_get_language()));

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
  locale_deinit();
  enamel_deinit();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, _("Done initializing, pushed window: %p"), window);

  app_event_loop();
  deinit();
}
