# esp32 log backtrace on restart

lifted from https://kotyara12.ru/pubs/iot/remote_esp32_backtrace/

## restart info
```
CPU0  Software reset CPU
CPU1  Software reset CPU
HEAP total 371040 free 323036 min 317524
STACK 400d1b3b 400e2be3 40083561 40085d92 400831f1 4008ac55 400904c1 400d1a16 400d1a1f 400d1a27 400d1a38 400d90f1 
```


## decode backtrace

```
$ xtensa-esp32-elf-addr2line -pfiaC -e .pio/build/m5stack-core-esp32/firmware.elf     400d1b3b 400e2be3 40083561 40085d92 400831f1 4008ac55 400904c1 400d1a16 400d1a1f 400d1a27 400d1a38 400d90f1

.. these are artefacts:

0x400d1b3b: __wrap_esp_panic_handler at ... src/restart.cpp:69
0x400e2be3: panic_handler at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_system/port/panic_handler.c:188
0x40083561: xt_unhandled_exception at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_system/port/panic_handler.c:219
0x40085d92: _xt_user_exc at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port/xtensa/xtensa_vectors.S:703
0x400831f1: panic_abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_system/panic.c:402
0x4008ac55: esp_system_abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp_system/esp_system.c:128
0x400904c1: __assert_func at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/newlib/assert.c:85

... the interesting part:

-> 0x400d1a16: third() at ... src/main.cpp:33
-> 0x400d1a1f: second() at ... src/main.cpp:34
-> 0x400d1a27: first() at ... src/main.cpp:35
-> 0x400d1a38: loop() at ... src/main.cpp:41
-> 0x400d90f1: loopTask(void*) at ... .platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:50

```