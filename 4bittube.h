#ifndef _4_BIT_TUBE_
#define _4_BIT_TUBE_

extern const uint8_t font4bit[];
//extern uint8_t *displayBuf;

uint8_t *Init_4bittube(
        uint8_t _displayCount,
        uint8_t _data,
        uint8_t _clock,
        uint8_t _latch);

#endif
