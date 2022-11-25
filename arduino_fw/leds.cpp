#include <stdint.h>
#include <Arduino.h>

#include "tick-led.h"
#include "leds.h"

//WS2812C timing  (different from WS2812B timing !)
//bit 0 : 220-380 H / 580-1000 L
//bit 1 : 580-1000 H / 580-100 L


//T0: 305ns
#define T0 {d1t = 3; while (d1t--) __asm ("nop"); }

//T1: 760ns
#define T1 {d1t = 10; while (d1t--) __asm ("nop"); }

#define T3 {d1t = 20; while (d1t--) __asm ("nop"); }


extern volatile uint8_t leds[MAX_LEDS];
extern ClockConfig clockCfg;

static uint8_t OUTER_RING[] = { 
    9,10,11,12,
    20,21,22,23,
    31,32,33,34,
    42,43,44,45,
    53,54,55,56,
    64,65,66,67,
    75,76,77,78,
    86,87,88,89,
    97,98,99,100,
    108,109,110,111,
    119,120,121,122,
    130,131,132,133
};

static uint8_t RING_2[] = {
    3,14,25,36,47,58,69,80,91,102,113,124
};

static uint8_t TOP_WEDGE[] = {
    1,2,3,4,5, 6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    123,124,125,126,127,128,129,130,
    131,132,133
};

static uint32_t cpalette[] = {
    0xe00000, 0xe00500, 0xde1500, 0xdc2700, 0xdc3e00, 0xd95600, 0xdb6900, 0xdb7e00, 0xe09300,
    0xe0a900, 0xe3be00, 0xe4d100, 0xe9e905, 0xc7d600, 0xacc600, 0x8ab200, 0x66a200, 0x479200,
    0x277e00, 0x156d14, 0x005336, 0x003c5e, 0x002988, 0x001cb9, 0x0008dc, 0x0003cf, 0x0000c2,
    0x0500b2, 0x17009d, 0x2c0087, 0x460076, 0x590053, 0x760039, 0x92001d, 0xaf0005, 0xc90000,
};


void getColorPalette(uint16_t index, uint32_t* result,  uint16_t offset) {
    index %= 36;
    if (NULL == result) {
        return;
    }
    result[1] = cpalette[index];
    index = (index + 9 + offset) % 36;
    result[2] = cpalette[index];
    index = (index + 9 + offset) % 36;
    result[3] = cpalette[index];

    result[0] = colorBlend(result[2], 0xBBBBBB, 0x80);
}

static inline IRAM_ATTR void setByte(uint8_t b) {
  volatile uint8_t d1t;
  uint16_t i = 8;

  while(i) {

    i--;
    // set the GPIO high
    SET_IO_PA(M_DATA);
    if (b & 0x80) {
      // wait while High is on
      T1;
      // set the GPIO low
      CLR_IO_PA(M_DATA);
      //wait while Low in on 
      T1;     
    } else {
      // wait while High is on
      T0;
      // set the GPIO low
      CLR_IO_PA(M_DATA);
      //wait while Low in on 
      T1;     
    }  
    b <<= 1;
  }
}

static IRAM_ATTR void setColor(uint8_t r, uint8_t g, uint8_t b) {
  volatile uint8_t d1t;
   
  setByte(g);
  setByte(r);
  setByte(b);

/*  T3; */
}

void ledsApply(void) {
    uint16_t i;
    uint16_t index = 0;
    
    portDISABLE_INTERRUPTS();
    for (i = 0; i < 133; i++) {      
        setColor(leds[index++], leds[index++], leds[index++]);
    }
    portENABLE_INTERRUPTS();

}

void ledsClear(int from) {
    int i;
    for (i = from; i < MAX_LEDS; i++) {
        leds[i] = 0;
    }
}

static void ledSet(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    index -= 1;
    index *= 3;
    leds[index++] = r;
    leds[index++] = g;
    leds[index] = b;
}

void ledSetSingle(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    ledSet(index, r, g, b);
}

void ledBlend(uint16_t index, uint16_t r, uint16_t g, uint16_t b, uint16_t a) {
    uint16_t a1 = 255 - a;
    uint16_t r1, g1, b1;
    
    index -= 1;
    index *= 3;

    r1 = leds[index] * a1;
    g1 = leds[index + 1] * a1;
    b1 = leds[index + 2] * a1;
    
    leds[index++] = ((r * a) + r1) >> 8;
    leds[index++] = ((g * a) + g1) >> 8;
    leds[index] = ((b * a) + b1) >> 8;
}

uint32_t colorBlend(uint32_t c1, uint32_t c2, uint16_t a2) {
    uint16_t r1, g1, b1;
    uint16_t r2, g2, b2;
    uint16_t a1;

    if (a2 == 0) {
        return c1;
    }
    if (a2 >= 0xFF) {
        return c2;
    }
    a1 = 255 - a2;
    r1 = ((c1 >> 16) & 0xFF) * a1;
    g1 = ((c1 >> 8) & 0xFF) * a1;
    b1 = (c1 & 0xFF) * a1;

    r2 = ((c2 >> 16) & 0xFF) * a2;
    g2 = ((c2 >> 8) & 0xFF) * a2;
    b2 = (c2 & 0xFF) * a2;

    //Serial.printf(" blend a2=%i result = 

    return (((r2 + r1) & 0xFF00) << 8) | ((g2 + g1) & 0xFF00) | (((b2 + b1) & 0xFF00) >> 8);
}

static void ledAdd(uint16_t index, uint16_t r, uint16_t g, uint16_t b) {
    uint16_t r1, g1, b1;
    
    index -= 1;
    index *= 3;

    r1 = leds[index] + r;
    g1 = leds[index + 1] + g;
    b1 = leds[index + 2] + b;

    if (r1 > 255) r1 = 255;
    if (g1 > 255) g1 = 255;
    if (b1 > 255) b1 = 255;
    
    if (r1 < 0) r1 = 0;
    if (g1 < 0) g1 = 0;
    if (b1 < 0) b1 = 0;
    
    leds[index++] = r1;
    leds[index++] = g1;
    leds[index] = b1;
}

static void ledSetArray(uint8_t* arr, int count, uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    int i;

    for (i = 0; i < count; i++) {
        ledSet(arr[i], r,g,b);
    }
}

void ledsSetOuterRing(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    if (clockCfg.oRingStyle == ORING_STYLE_12_DOTS) {
        ledSet(9, r,g,b);
        ledSet(20, r,g,b);
        ledSet(31, r,g,b);
        ledSet(42, r,g,b);
        ledSet(53, r,g,b);
        ledSet(64, r,g,b);
        ledSet(75, r,g,b);
        ledSet(86, r,g,b);
        ledSet(97, r,g,b);
        ledSet(108, r,g,b);
        ledSet(119, r,g,b);
        ledSet(130, r,g,b);
    } else if (clockCfg.oRingStyle == ORING_STYLE_4_DOTS) {
        ledSet(9, r,g,b);
        ledSet(42, r,g,b);
        ledSet(75, r,g,b);
        ledSet(108, r,g,b);       
    }
}

void ledsSetInnerRing(uint32_t color) {
    ledSetArray(RING_2, sizeof(RING_2), color);
}

void ledsSetWedge(uint32_t color) {
    ledSetArray(TOP_WEDGE, sizeof(TOP_WEDGE), color);
}

void ledsSetSpoke(uint16_t spokeIndex, uint32_t color, uint8_t ledCnt, uint8_t* blinds, uint8_t blindCnt) {
    uint16_t i;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    if (spokeIndex >= 12) {
        spokeIndex -= 12;
    }
    if (ledCnt == 0) {
           return;
    }
    spokeIndex *= 11;
    spokeIndex += 2;

    for (i = spokeIndex; i < spokeIndex + ledCnt; i++) {
        ledSet(i, r,g,b);
    }

    if (blinds != NULL || blindCnt > 0) {
        i = 0;
        while (i < blindCnt) {
            ledSet(spokeIndex + blinds[i++], 0,0,0);
        }
    }
}



void ledsSetSpokeMin(uint16_t spokeIndex, uint16_t minIndex, uint32_t color, uint8_t ledCnt) {
    uint16_t i;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    if (spokeIndex >= 12) {
        spokeIndex -= 12;
    }
    spokeIndex *= 11;
    spokeIndex += 2;

    spokeIndex += ledCnt;

    if (clockCfg.minuteStyle == MIN_STYLE_FULL_BAR) {
        for (i = spokeIndex; i < spokeIndex + minIndex; i++) {
            ledSet(i, r,g,b);
        }
    } else if (clockCfg.minuteStyle == MIN_STYLE_DOT) {
        i = spokeIndex + minIndex - 1;
        ledSet(i, r,g,b);
    }
}


void ledsSetSeconds(int s, uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    int p0, p1, p2, p3;
    float point =  s / 1200.0f; // 1200 = 60 seconds * 20 animation steps
    point *= 48; // 48 LEDs on the outer ring

    if (clockCfg.secondStyle == SEC_STYLE_FLOATING) {
        p1 = point;
        p0 = point - 1;
        if (p0 < 0) {
            p0 = 47;
        }
        p2 = (point * 50) - (p1 * 50);
        p3 = 50 - p2;
        //ledBlend(OUTER_RING[p1], 30, 30, 100, p2);
        //ledBlend(OUTER_RING[p0], 30, 30, 100, p3);
        ledAdd(OUTER_RING[p1], (r * p2) >> 7, (g * p2) >> 7, (b * p2) >> 7);
        ledAdd(OUTER_RING[p0], (r * p3) >> 7, (g * p3) >> 7, (b * p3) >> 7);
    } else if (clockCfg.secondStyle == SEC_STYLE_DOT) {
        p1 = point;
        p2 = (point * 50) - (p1 * 50);
        ledAdd(OUTER_RING[p1], (r * p2) >> 7, (g * p2) >> 7, (b * p2) >> 7);       
    }
}
