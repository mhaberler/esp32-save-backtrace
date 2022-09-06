

#include "restart.h"
#include <M5Unified.h>

void print_stackaddr(uint32_t addr) { Serial.printf("%x ", addr); }

void setup() {
  esp_register_shutdown_handler(update_debug_info);

  M5.begin();

  Serial.printf("CPU0  %s\n", get_reset_reason(0));
  Serial.printf("CPU1  %s\n", get_reset_reason(1));

  re_heapinfo_t *hi;
  if ((hi = get_heapinfo()) != NULL) {
    Serial.printf("HEAP total %zu free %zu min %zu\n", hi->heap_int_total,
                  hi->heap_int_free, hi->heap_int_free_min);
  }
  if (get_backtrace()) {
    Serial.printf("STACK ");
    get_backtrace(print_stackaddr);
    Serial.printf("\n");
  }
  clear_restart_info();

  M5.Lcd.print("Hello world!");
}

void third() { assert(0); }
void second() { third(); }
void first() { second(); }

void loop() {
  update_heap_info();
  M5.update();
  first();
}
