#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include <uart.h>
#include <i2c_master.h>

/* Enable printing by default */
#ifndef ENABLE_UART_MSG
#  define ENABLE_UART_MSG 1
#endif

#if ENABLE_UART_MSG
#  define print_init()     uart_init(BIT_RATE_115200, BIT_RATE_115200);
#  define print(...)  os_printf(__VA_ARGS__) 
#else
#  define print_init()
#  define print(...)
#endif


/* 0x30 is the default of all mcp9808.
   It's (0x18 << 1) and lower bit is read/write bit. */

#define MCP9808_ADDRESS      (0x18 << 1)

#define Q9_4_MASK            0x1FFF

#define INVALID_TEMPERATURE  ((int)0x80000000)

static int
read_mcp9808()
{
  int t = INVALID_TEMPERATURE;
  
  i2c_master_start();
  
  /* Select device on i2c */
  i2c_master_writeByte(MCP9808_ADDRESS);
  if (i2c_master_getAck()) {
    print("i2c mcp9808 address write failed\n\r");
  } else {
    
    /* Write TA Register address */
    i2c_master_writeByte(0x05);
    if (i2c_master_getAck()) {
      print("i2c Ta register write failed\n\r");
    } else {
      
      /* Repeat start */
      i2c_master_start();
      /* Sending READ command */
      i2c_master_writeByte(MCP9808_ADDRESS | 0x01);
      if (i2c_master_getAck()) {
	print("i2c Ta READ command failed\n\r");
      } else {
	unsigned upper, lower;
	/* Read temperature */
	upper = i2c_master_readByte();
	i2c_master_send_ack ();
	lower = i2c_master_readByte();
	i2c_master_send_nack ();

	/* Sign extend to int */
	t = (upper & 0x10) ? (-1 - Q9_4_MASK) : 0;
	/* Put Q9.4 into lower bits */
	t |= ((upper << 8) | lower) & Q9_4_MASK;
      }
    }
  }
  
  /* Done, send STOP command. */
  i2c_master_stop();

  return t;
}

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

os_event_t user_procTaskQueue[user_procTaskQueueLen];
static void loop(os_event_t *events);

static void ICACHE_FLASH_ATTR
loop(os_event_t *events)
{
  static int i=1;

  int t = read_mcp9808();

  /* No printing of floats available :(, t is in signed Q9.4 format,
     printing is correct only for t>=0. */
  print("Ho Ho Ho %d, temperature: %d.%04d (raw: %x)\n\r", i++, t/16, 625*(t&15), t);

  os_delay_us(100000);
  system_os_post(user_procTaskPrio, 0, 0 );
}

void ICACHE_FLASH_ATTR
user_init()
{
  print_init();
  gpio_init();
  i2c_master_gpio_init();
  
  os_delay_us(5000);

  /* Start os task */
  system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
  system_os_post(user_procTaskPrio, 0, 0 );
}
