//
// Code to drive the cheap 4-digit LED display modules from QIFEI.
// labelled 4-Bit LED Digital Tube Module
//
// Project is available at https://github.com/SmallRoomLabs/4bittube
//
// Copyright 2017 Mats Engstrom, Released under the MIT licence
//

#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "gpio.h"

#include "4bittube.h"

// Functions missing in SDK declarations :-(
void *pvPortZalloc(size_t, const char * file, int line);
void ets_timer_disarm(ETSTimer *a);
void ets_timer_setfn(ETSTimer *t, ETSTimerFunc *fn, void *pArg);
void ets_timer_arm_new(ETSTimer *a, int b, int c, int isMstimer);

// Local static storage
static uint8_t pinData;
static uint8_t pinClock;
static uint8_t pinLatch;
static uint8_t displayCount;

// Timer for refreshing the displays
static os_timer_t ledRefreshTimer;

// pointer to malloced display memory
static uint8_t *displayBuf;

// 7-segment "Font" for numbers and some punctuation
const uint8_t font4bit[]= {
  0x3F, 0x06, 0x5B, 0x4F, // 0123
  0x66, 0x6d, 0x7D, 0x07, // 4567
  0x7F, 0x6F, 0x77, 0x7C, // 89AB
  0x58, 0x5E, 0x79, 0x71, // CDEF
  0x40, 0x80, 0x00        // -.<blank>
};

//
// Shift out one byte of data to the display (chain)
// Since these displays have inverted the logic level
// of the segments (0=on 1=off) we invert the data 
// while shifting instead of letting the caller handle
// the inverted data
//
static void ShiftOut(uint8_t data) {
  uint8_t mask;
  for (mask=0x80; mask; mask>>=1) {
    if (data & mask) {
      GPIO_OUTPUT_SET(pinData,0);  
    } else {
      GPIO_OUTPUT_SET(pinData,1);  
    }
    GPIO_OUTPUT_SET(pinClock,0);  
    GPIO_OUTPUT_SET(pinClock,1);
  }  
}

//
// Timer callback function that refreshes the LED display
//
void ledRefreshCB(void *arg) {
  static uint8_t digit;
  uint8_t dc;
  uint8_t i;

  if (digit++>3) digit=0;

  i=displayCount*4-1-digit;
  // Send segments & digit for each connected display
  for (dc=0; dc<displayCount; dc++) {
    ShiftOut(displayBuf[i]);    // Segments (inverted), LSB=Segment A, 
    i-=4;
    ShiftOut((1<<digit)^0xFF);  // Digit#, LSB=Rightmost digit
  }

  // Latch the data by a brief low-going pulse
  GPIO_OUTPUT_SET(pinLatch,0);  
  GPIO_OUTPUT_SET(pinLatch,1);
}


//
// Inittalize and setup the displays.
//
uint8_t * ICACHE_FLASH_ATTR Init_4bittube(
    uint8_t _displayCount,
    uint8_t _data, 
    uint8_t _clock, 
    uint8_t _latch) {

  // Keep track of the GPIO pins connected to the display
  pinData=_data;
  pinClock=_clock;
  pinLatch=_latch;
  displayCount=_displayCount;

  // Set the display GPIOs as outputs
  GPIO_OUTPUT_SET(pinData,1);
  GPIO_OUTPUT_SET(pinClock,1);
  GPIO_OUTPUT_SET(pinLatch,1);

  // Allocate memory for the display buffer
  displayBuf=(uint8_t *)os_zalloc(displayCount*4);

  // Create and start a timer for refreshing the LED display 
  // 5 ms gives 1000/(5*4) Hz = 50Hz refresh for a reasonably flicker-
  // free display
  os_timer_disarm(&ledRefreshTimer);
  os_timer_setfn(&ledRefreshTimer, (os_timer_func_t *)ledRefreshCB, NULL);
  os_timer_arm(&ledRefreshTimer, 5, 1);

  // Return the allocated display buffer
  return displayBuf;
}

