#include <pebble.h>
#define KEY_CITY 0 
#define KEY_RANDOM 1  

static Window *s_main_window;
static TextLayer *s_time_layer, *s_city_layer, *s_date_layer, *s_battery_layer;
static InverterLayer *s_night_layer;
static BitmapLayer *s_background_layer, *s_star_layer;
static Layer *s_line_layer;
bool RandomFlag = true;
char key_random[] = "1";

static GFont s_time_font;
static GBitmap *s_background_bitmap, *s_star_bitmap;
static char s_date_buffer[10];
char citystr[] = "_SAN_FRANCISCO_";
static int backgr_res;

const int NBR_OF_CITIES = 29;
char *citylist[29] = {"Athens", "Barcelona", "Berlin", "Boston", "Brussels", "Budapest", "Cairo", "Chicago", "Dallas", "Delhi", "Dublin", "Istanbul", "Kiev", "Kyoto", "London", "Moscow", "New York", "Paris", "Pisa", "Prague", "Rome", "Saint-Louis", "San Francisco", "Seattle", "Shangai", "Singapore", "Sydney", "Tokyo", "Washington"};

static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%% ", new_state.charge_percent);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}


void drawtimeline(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static int resource_background(char citystring[]) {
  if (strcmp(citystr, "Paris")              == 0) { return RESOURCE_ID_PARIS_BACKGROUND;     }
  else if (strcmp(citystr, "London")        == 0) { return RESOURCE_ID_LONDON_BACKGROUND;    }
  else if (strcmp(citystr, "Boston")        == 0) { return RESOURCE_ID_BOSTON_BACKGROUND;    } 
  else if (strcmp(citystr, "Brussels")      == 0) { return RESOURCE_ID_BRUSSELS_BACKGROUND;  }  
  else if (strcmp(citystr, "Prague")        == 0) { return RESOURCE_ID_PRAGUE_BACKGROUND;    } 
  else if (strcmp(citystr, "New York")      == 0) { return RESOURCE_ID_NEWYORK_BACKGROUND;   }
  else if (strcmp(citystr, "Sydney")        == 0) { return RESOURCE_ID_SYDNEY_BACKGROUND;    }
  else if (strcmp(citystr, "San Francisco") == 0) { return RESOURCE_ID_FRISCO_BACKGROUND;    } 
  else if (strcmp(citystr, "Tokyo")         == 0) { return RESOURCE_ID_TOKYO_BACKGROUND;     } 
  else if (strcmp(citystr, "Rome")          == 0) { return RESOURCE_ID_ROME_BACKGROUND;      } 
  else if (strcmp(citystr, "Saint Louis")   == 0) { return RESOURCE_ID_SLOUIS_BACKGROUND;    } 
  else if (strcmp(citystr, "Barcelona")     == 0) { return RESOURCE_ID_BARCA_BACKGROUND;     } 
  else if (strcmp(citystr, "Kiev")          == 0) { return RESOURCE_ID_KIEV_BACKGROUND;      } 
  else if (strcmp(citystr, "Istanbul")      == 0) { return RESOURCE_ID_ISTANBUL_BACKGROUND;  } 
  else if (strcmp(citystr, "Moscow")        == 0) { return RESOURCE_ID_MOSCOW_BACKGROUND;    } 
  else if (strcmp(citystr, "Seattle")       == 0) { return RESOURCE_ID_SEATTLE_BACKGROUND;   } 
  else if (strcmp(citystr, "Chicago")       == 0) { return RESOURCE_ID_CHICAGO_BACKGROUND;   } 
  else if (strcmp(citystr, "Singapore")     == 0) { return RESOURCE_ID_SINGAPORE_BACKGROUND; } 
  else if (strcmp(citystr, "Berlin")        == 0) { return RESOURCE_ID_BERLIN_BACKGROUND;    } 
  else if (strcmp(citystr, "Dublin")        == 0) { return RESOURCE_ID_DUBLIN_BACKGROUND;    } 
  else if (strcmp(citystr, "Shangai")       == 0) { return RESOURCE_ID_SHANGAI_BACKGROUND;   } 
  else if (strcmp(citystr, "Dallas")        == 0) { return RESOURCE_ID_DALLAS_BACKGROUND;    } 
  else if (strcmp(citystr, "Kyoto")         == 0) { return RESOURCE_ID_KYOTO_BACKGROUND;     }
  else if (strcmp(citystr, "Pisa")          == 0) { return RESOURCE_ID_PISA_BACKGROUND;      } 
  else if (strcmp(citystr, "Cairo")         == 0) { return RESOURCE_ID_CAIRO_BACKGROUND;     } 
  else if (strcmp(citystr, "Delhi")         == 0) { return RESOURCE_ID_DELHI_BACKGROUND;     } 
  else if (strcmp(citystr, "Athens")        == 0) { return RESOURCE_ID_ATHENS_BACKGROUND;    }
  else if (strcmp(citystr, "Budapest")      == 0) { return RESOURCE_ID_BUDAPEST_BACKGROUND;  } 
  else if (strcmp(citystr, "Washington")    == 0) { return RESOURCE_ID_WASHINGTON_BACKGROUND;}  
  else {
    // fallback config if invalid KEY_CITY config
    strcpy(citystr,"Prague");
    return RESOURCE_ID_PRAGUE_BACKGROUND;
  }  
}


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) { strftime(buffer, sizeof("00:00"), "%H:%M", tick_time); } 
  else { strftime(buffer, sizeof("00:00"), "%I:%M", tick_time); }

  // Display time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  // Display date on DateLayer
  strftime(s_date_buffer, sizeof(s_date_buffer), "%a %d", tick_time);
  text_layer_set_text(s_date_layer, s_date_buffer);
  
  // Nighty-night
  if ((tick_time->tm_hour >= 20) || (tick_time->tm_hour <6)) {
    //Activating night mode
    if (layer_get_hidden((Layer *)s_night_layer)) { 
      layer_set_hidden((Layer *)s_night_layer, false);
      layer_set_hidden((Layer *)s_star_layer, false);
    }
  }  
  else {
    // Deactivating night mode
    if (! layer_get_hidden((Layer *)s_night_layer)) { 
      layer_set_hidden((Layer *)s_night_layer, true);
      layer_set_hidden((Layer *)s_star_layer, true); 
    }
  }
  
  // Random, change every 2 hours
  if (RandomFlag) {
    // Build a better rand()
    srand((unsigned) time(&temp));
    
    if ((((tick_time->tm_hour) % 2) != 0) && (tick_time->tm_min == 0)) {
      int r = rand() % NBR_OF_CITIES;
      strcpy(citystr, citylist[r]);
      backgr_res = resource_background(citystr);
      text_layer_set_text(s_city_layer, citystr);
      s_background_bitmap = gbitmap_create_with_resource(backgr_res);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    }
  }
}



static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  //Get data
  Tuple *t = dict_read_first(iterator);
  
  while (t) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG] Found dict key=%i", (int)t->key);
   switch(t->key) {

    case KEY_RANDOM:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG]   KEY_RANDOM=%s", t->value->cstring);
      if (strcmp(t->value->cstring, "0") == 0) { RandomFlag = false; }
      break;
     
    case KEY_CITY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG]   KEY_CITY=%s", t->value->cstring);
      strcpy(citystr,t->value->cstring); 
      //update layers
      text_layer_set_text(s_city_layer, citystr);
      backgr_res = resource_background(citystr); 
      s_background_bitmap = gbitmap_create_with_resource(backgr_res);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
      persist_write_string(KEY_CITY,citystr);
      break;
   }
    
   t = dict_read_next(iterator);
    
  }
}


static void inbox_dropped(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG] inbox_dropped %d", reason);
}


static void main_window_load(Window *window) {
  // Load user config
  if (persist_exists(KEY_CITY)) { 
    persist_read_string(KEY_CITY, citystr, sizeof(citystr));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG] persistent KEY_CITY found:%s ", citystr);
  }
  else {
    // fallback config if no config found
    strcpy(citystr,"Prague"); 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG] no KEY_CITY, setting Prague as default");
  }
  
  if (persist_exists(KEY_RANDOM)) {
    persist_read_string(KEY_RANDOM, key_random, sizeof(key_random));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG] persistent KEY_RANDOM found:%s ", key_random);
    if (strcmp(key_random, "0") == 0) { RandomFlag = false; }
  }
  backgr_res = resource_background(citystr);  
  s_background_bitmap = gbitmap_create_with_resource(backgr_res);
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 5, 139, 55));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create battery TextLayer
  s_battery_layer = text_layer_create(GRect(22, 63, 50, 17));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_text(s_battery_layer, "100%");

  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(73, 63, 50, 17));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "Sat 01");
  
  //Create GBitmap, then set to created BitmapLayer
  s_background_layer = bitmap_layer_create(GRect(0, 80, 144, 68));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create city TextLayer
  s_city_layer = text_layer_create(GRect(0, 148, 144, 20));
  text_layer_set_background_color(s_city_layer, GColorBlack);
  text_layer_set_text_color(s_city_layer, GColorWhite);
  text_layer_set_text(s_city_layer, citystr);
    
  //Time
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POCKET_CALCULATOR_SIZE_54));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  //City
  //s_city_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TYPEONE_SQUARE_12));
  text_layer_set_font(s_city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  //text_layer_set_font(s_city_layer, s_time_font);
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_city_layer));
  
  //Date
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  //Battery
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));

  //Draw line under time
  s_line_layer = layer_create(GRect(22,62,102,1));
  layer_add_child(window_get_root_layer(window), s_line_layer);
  layer_set_update_proc(s_line_layer,drawtimeline);
  
  // Nightlayer
  s_star_layer = bitmap_layer_create(GRect(0,0,144,120));
  s_star_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_NIGHT_STARS);
  bitmap_layer_set_bitmap(s_star_layer,s_star_bitmap);
  bitmap_layer_set_compositing_mode(s_star_layer,GCompOpAnd);
  layer_set_hidden((Layer *)s_star_layer, true);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_star_layer));
  
  s_night_layer = inverter_layer_create(GRect(0,0,144,168));
  layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(s_night_layer));
  layer_set_hidden((Layer *)s_night_layer,true);
  
  // Get the current battery level
  battery_handler(battery_state_service_peek());
  
  // Make sure the time is displayed from the start
  update_time();
}


static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_time_font);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_city_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_battery_layer);  
  layer_destroy(s_line_layer);
  inverter_layer_destroy(s_night_layer);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  

static void init() {
  app_message_register_inbox_received(in_recv_handler);
  app_message_register_inbox_dropped(inbox_dropped);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
}


static void deinit() {
  // Write persistent data
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[DBUG] Writing persistent data...");
  persist_write_string(KEY_CITY, citystr);
  if (! RandomFlag) { strcpy(key_random, "0"); }
  persist_write_string(KEY_RANDOM, key_random);
  
  // Destroy Window
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}
