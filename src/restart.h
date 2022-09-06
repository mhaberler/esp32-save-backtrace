#include <M5Unified.h>

#define CONFIG_RESTART_DEBUG_INFO 1
#define CONFIG_RESTART_DEBUG_STACK_DEPTH 32

#define RESTART_HEAP_MAGIC 0xDEADBEEF
#define RESTART_STACK_MAGIC 0xBEEFDEAD

typedef struct {

  size_t heap_total;
  size_t heap_free;
  size_t heap_free_min;

  size_t heap_int_total;
  size_t heap_int_free;
  size_t heap_int_free_min;
} re_heapinfo_t;

typedef struct {
  uint32_t backtrace[CONFIG_RESTART_DEBUG_STACK_DEPTH];
} re_stackinfo_t;

typedef struct {
  uint32_t heap_magic;
  re_heapinfo_t hi;
  uint32_t stack_magic;
  re_stackinfo_t si;
} re_restart_debug_t;

typedef enum {
  RR_UNKNOWN = 0,
  RR_OTA = 1,
  RR_OTA_TIMEOUT = 2,
  RR_OTA_FAILED = 3,
  RR_COMMAND_RESET = 4,
  RR_HEAP_ALLOCATION_FAILED = 5,
  RR_WIFI_TIMEOUT = 6,
  RR_BAT_LOW = 7
} re_reset_reason_t;

typedef void (*get_addr)(uint32_t);

extern void IRAM_ATTR update_debug_info();
extern void IRAM_ATTR update_heap_info();

extern void clear_restart_info();
extern re_heapinfo_t *get_heapinfo();
extern bool get_backtrace(get_addr func = NULL);
extern const char *get_reset_reason(int cpu_no);
