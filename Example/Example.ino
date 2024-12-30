#include "display.h"

void setup()
{
  setup_display();

  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello Arduino, I'm LVGL!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn1, NULL, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
  lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

  lv_obj_t * btn1label = lv_label_create(btn1);
  lv_label_set_text(btn1label, "Button");
  lv_obj_center(btn1label);
}

void loop()
{
  loop_display();
}
