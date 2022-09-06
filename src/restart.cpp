
#include "restart.h"

#if CONFIG_RESTART_DEBUG_INFO

#include "esp_attr.h"
#include "esp_debug_helpers.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_types.h"
#include "soc/cpu.h"
#include "soc/soc_memory_layout.h"
#include <esp32/rom/rtc.h>

__NOINIT_ATTR static re_restart_debug_t _debug_info;

extern "C" void __real_esp_panic_handler(void *info);

void IRAM_ATTR update_heap_info() {
  _debug_info.heap_magic = RESTART_HEAP_MAGIC;
  _debug_info.hi.heap_total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
  _debug_info.hi.heap_free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  size_t _new_free_min = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
  if ((_debug_info.hi.heap_free_min == 0) ||
      (_new_free_min < _debug_info.hi.heap_free_min)) {
    _debug_info.hi.heap_free_min = _new_free_min;
  };
  _debug_info.hi.heap_int_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
  _debug_info.hi.heap_int_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  _new_free_min = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
  if ((_debug_info.hi.heap_int_free_min == 0) ||
      (_new_free_min < _debug_info.hi.heap_int_free_min)) {
    _debug_info.hi.heap_int_free_min = _new_free_min;
  };
}

void IRAM_ATTR update_backtrace_info() {
  esp_backtrace_frame_t stk_frame;
  esp_backtrace_get_start(&(stk_frame.pc), &(stk_frame.sp),
                          &(stk_frame.next_pc));
  _debug_info.si.backtrace[0] = esp_cpu_process_stack_pc(stk_frame.pc);

  bool corrupted =
      (esp_stack_ptr_is_sane(stk_frame.sp) &&
       esp_ptr_executable((void *)esp_cpu_process_stack_pc(stk_frame.pc)))
          ? false
          : true;

  uint8_t i = CONFIG_RESTART_DEBUG_STACK_DEPTH;
  while (i-- > 0 && stk_frame.next_pc != 0 && !corrupted) {
    if (!esp_backtrace_get_next_frame(&stk_frame)) {
      corrupted = true;
    };
    _debug_info.stack_magic = RESTART_STACK_MAGIC;
    _debug_info.si.backtrace[CONFIG_RESTART_DEBUG_STACK_DEPTH - i] =
        esp_cpu_process_stack_pc(stk_frame.pc);
  };
}

void IRAM_ATTR update_debug_info() {
  update_heap_info();
  update_backtrace_info();
}

// This function will be considered the esp_panic_handler to call in case a
// panic occurs
extern "C" void __wrap_esp_panic_handler(void *info) {

  update_backtrace_info();
  // Call the original panic handler function to finish processing this error
  // (creating a core dump for example...)
  __real_esp_panic_handler(info);
}

void clear_restart_info() { memset(&_debug_info, 0, sizeof(_debug_info)); }

re_heapinfo_t *get_heapinfo() {
  esp_reset_reason_t esp_reason = esp_reset_reason();
  if ((esp_reason != ESP_RST_UNKNOWN) && (esp_reason != ESP_RST_POWERON) &&
      (_debug_info.heap_magic == RESTART_HEAP_MAGIC)) {
    return &_debug_info.hi;
  }
  return NULL;
}

bool get_backtrace(get_addr f) {
  if (_debug_info.stack_magic != RESTART_STACK_MAGIC)
    return false;
  for (uint8_t i = 0; i < CONFIG_RESTART_DEBUG_STACK_DEPTH; i++) {
    uint32_t addr = _debug_info.si.backtrace[i];
    if ((f == NULL) && (addr != 0))
        return true;
    if (f && addr)
      (*f)(addr);
  }
  return false;
}

const char *get_reset_reason(int cpu_no) {
  RESET_REASON esp_reason = rtc_get_reset_reason(cpu_no);
  switch (esp_reason) {
  case POWERON_RESET:
    return "Vbat power on reset";
  case SW_RESET:
    return "Software reset digital core";
  case OWDT_RESET:
    return "Legacy watch dog reset digital core";
  case DEEPSLEEP_RESET:
    return "Deep Sleep reset digital core";
  case SDIO_RESET:
    return "Reset by SLC module, reset digital core";
  case TG0WDT_SYS_RESET:
    return "Timer Group0 Watch dog reset digital core";
  case TG1WDT_SYS_RESET:
    return "Timer Group1 Watch dog reset digital core";
  case RTCWDT_SYS_RESET:
    return "RTC Watch dog Reset digital core";
  case INTRUSION_RESET:
    return "Instrusion tested to reset CPU";
  case TGWDT_CPU_RESET:
    return "Time Group reset CPU";
  case SW_CPU_RESET:
    return "Software reset CPU";
  case RTCWDT_CPU_RESET:
    return "RTC Watch dog Reset CPU";
  case EXT_CPU_RESET:
    return "For APP CPU, reseted by PRO CPU";
  case RTCWDT_BROWN_OUT_RESET:
    return "Reset when the vdd voltage is not stable";
  case RTCWDT_RTC_RESET:
    return "RTC Watch dog reset digital core and rtc module";
  default:
    return "NO MEAN";
  };
}

#endif
