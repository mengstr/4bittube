//
// Example code for using the "4bittube" code to
// the cheap 4-digit LED display modules
//
// Project is available at https://github.com/SmallRoomLabs/4bittube
//

#include "c_types.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"

#include "4bittube.h"

// Functions missing in SDK declarations :-(
void ets_delay_us(int ms);

// GPIO-pins on the NodeMCU connected to the display modules
#define TUBE_DATA   5   // D1 GPIO5
#define TUBE_LATCH  4   // D2 GPIO4
#define TUBE_CLOCK  0   // D3 GPIO0
// Number of modules connected in the chain
#define TUBE_COUNT  3   // We have three modules in the chain

// Display buffer - memory allocated by the Init_4bittube() function
uint8_t *disp; 

// Defines for the main processing task
#define procTaskPrio        0
#define procTaskQueueLen    1
os_event_t procTaskQueue[procTaskQueueLen];
void procTask(os_event_t *events);

// The main procsssing task. Currently empty.
void ICACHE_FLASH_ATTR procTask(os_event_t *events) {
  system_os_post(procTaskPrio, 0, 0 );
  os_delay_us(10);
}

//
// Init/Setup function that gets run once at boot.
//
void ICACHE_FLASH_ATTR user_init() {
  uint8_t i;

  // Initialize the modules
  disp=Init_4bittube(TUBE_COUNT, TUBE_DATA, TUBE_CLOCK, TUBE_LATCH);
  // Fill the display with numbers using the "font"
  for (i=0; i<TUBE_COUNT*4; i++) {
    disp[i]=font4bit[i];
  }

  // Start the main processing task that does nothing in this example
  system_os_task(procTask, 
                 procTaskPrio,
                 procTaskQueue, 
                 procTaskQueueLen);
  system_os_post(procTaskPrio, 0, 0 );

}
