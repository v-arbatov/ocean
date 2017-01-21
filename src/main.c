#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "uart.h"

/* Enable printing by default */
#ifndef ENABLE_UART_MSG
#  define ENABLE_UART_MSG 1
#endif

#if ENABLE_UART_MSG
#  define print_init()     uart_init(BIT_RATE_115200, BIT_RATE_115200);
#  define print(msg, ...)  os_printf(msg, __VA_ARGS__) 
#else
#  define print_init()
#  define print(msg, ...)
#endif

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

os_event_t user_procTaskQueue[user_procTaskQueueLen];
static void loop(os_event_t *events);

static void ICACHE_FLASH_ATTR
loop(os_event_t *events)
{
  static int i=1;
  print("Ho Ho Ho %d\n\r", i++);
  os_delay_us(100000);
  system_os_post(user_procTaskPrio, 0, 0 );
}

void ICACHE_FLASH_ATTR
user_init()
{
  print_init();
  gpio_init();
  
  os_delay_us(5000);

  /* Start os task */
  system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
  system_os_post(user_procTaskPrio, 0, 0 );
}
