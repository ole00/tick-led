#ifndef _LEDS_H_
#define _LEDS_H_
void ledsClear(int from);
void ledBlend(uint16_t index, uint16_t r, uint16_t g, uint16_t b, uint16_t a);
uint32_t colorBlend(uint32_t c1, uint32_t c2, uint16_t a2);
void ledSetSingle(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void ledsApply(void);

//result - buffer of at least 4 eleements must be passed
void getColorPalette(uint16_t index, uint32_t* result, uint16_t offset);

// clock specific
void ledsSetOuterRing(uint32_t color);
void ledsSetSpoke(uint16_t spokeIndex, uint32_t color, uint8_t ledCnt, uint8_t* blinds, uint8_t blindCnt);
void ledsSetSpokeMin(uint16_t spokeIndex, uint16_t minIndex, uint32_t color, uint8_t ledCnt);
void ledsSetSeconds(int s, uint32_t color);
void ledsSetInnerRing(uint32_t color);
void ledsSetWedge(uint32_t color);

#endif
