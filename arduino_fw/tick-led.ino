/* 
   
   TICK-LED - an LED clock / alarm with buzzer an accelerometer
              Uses WS2812C RGB LEDs
              Uses ESP32 S2 MCU
              
              more info: https://github.com/ole00/tick-led
   ============================================================================
   
   
   USB UPLOAD - Via bootloader (press ob-board buttons)
                !!! do not use auto-reboot via serial!!!
  
*/


#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <soc/soc.h>
#include <soc/periph_defs.h>
#include <driver/timer.h>
#include <driver/ledc.h>
#include <Wire.h>

#include "time.h"
#include "Timezone.h"
#include "storage.h"
#include "soc/rtc_wdt.h"

#include "esp_log.h"

#include "tick-led.h"
#include "leds.h"
#include "network.h"
#include "config.h"
#include "rttl.h"
#include "rttl_tunes.h"

//#define ISR_TRACE


#if ARDUINO_USB_CDC_ON_BOOT == 0
#error USB CDC ON BOOT NOT DEFINED!
#endif

#if CONFIG_IDF_TARGET_ESP32S2 == 0
#error This code works only on ESP32-S2
#endif

#include <soc_log.h>


#define MAX_INPUT_LINE           62
#define COMMAND_NONE              0
#define COMMAND_UNKNOWN           1


#define BUZZ_MUTE 8
#define BUZZ_ON 1

//accelerometer I2C address
#define ACM_MMA8491Q_ADDR 0x55
#define ACM_MC3479_ADDR 0x4C
#define MOTION_THRESHOLD 12

#define ACM_INFO 0
//~ 20 minutes (6000 is ~ 1 minute)
#define CLOCK_CONFIG_TIMEOUT 120000;

uint8_t state = STATE_INIT;


static uint8_t BUBBLES1[] = {
    1, 6, 4, 0, 2, 5, 3, 1, 6, 4, 0, 2, 5, 3, 1, 6, 4, 0, 2
};

static uint8_t BUBBLES2[] = {
    3, 0, 4, 2, 1, 3, 0, 4, 2, 1, 3, 0, 4, 2, 1, 3, 0
};


char errorMessage[64];

uint32_t configExit;
uint32_t buttonPress = 0; 

volatile uint8_t leds[MAX_LEDS];

char inputLine[MAX_INPUT_LINE + 2];
uint16_t lineIndex;

char rttlTune[1024];


int16_t acx, acy, acz; //accelerometer values
uint8_t acInMotion; //acceleremometer in motion
uint8_t acType;

struct tm tim;
Timezone tz;
TimeChangeRule tzDst;
TimeChangeRule tzStd;
uint32_t colorSchemeOverride;
uint8_t doNtp;
uint8_t forceHandAnim;    // for preview hand animation (via button press)
uint16_t manualUndim;

uint16_t randSeed; // seed for random - taken from the mac address;
uint32_t clockConfigCounter;

typedef struct ColorScheme {
    uint8_t centerR;
    uint8_t centerG;
    uint8_t centerB;

    uint32_t outerRing;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} ColorScheme;

ClockConfig clockCfg;
ColorScheme colors[] = {
  //center dot         O ring      Hour     Minute     Second
  //default
  {0x3f, 0x3f, 0x3f,   0x1F0000,     0xFF,  0xA07000, 0x808080},
  //orange
  {0x20, 0x20, 0,      0x202000, 0xA0A0A0,  0xA03000, 0x808080},
  //fandango
  {0x3f, 0x0, 0x00,    0x200020, 0xFF00FF,  0x3000FF, 0x800000},
  //Red
  {0x3f, 0x3f, 0x3f,   0x200505, 0xA01020,  0xFF0000, 0x808080},
  //Green
  {0x3f, 0x3f, 0x3f,   0x202000, 0xA0A0A0,  0x00FF00, 0x808080},
  //Icy White
  {0x1f, 0x1f, 0x1f,   0x101020, 0xA0A0A0,  0x8080FF, 0x000080},
};
#define MAX_COLOR_SCHEME  5

ColorScheme randomColors;
ColorScheme blendColors;

static RttlContext rttl;

static void detectAcm(void);
static void updateAcm(void);
static void startSongIndex(uint8_t songIndex, uint16_t loops);


// Time zone Daylight Saving rules
// https://www.countries-ofthe-world.com/world-time-zones.html
static void setDsRules()
{
    Serial.printf("DS Rule:%d\r\n", clockCfg.dsRule);
    switch (clockCfg.dsRule) {
        case 1: // Central Europe
            tzDst = {Last, Sun, Mar, 2, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 3, clockCfg.tzOffset};
            break;
        case 2: // Eastern Europe
            tzDst = {Last, Sun, Mar, 3, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 4, clockCfg.tzOffset};
            break;
        case 3: // Western Europe
            tzDst = {Last, Sun, Mar, 1, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 2, clockCfg.tzOffset};
            break;
        case 4: // Azores (Portugal)
            tzDst = {Last, Sun, Mar, 0, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 1, clockCfg.tzOffset};
            break;
        case 5: // Greenland
            tzDst = {Last, Sun, Mar, 22, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 23, clockCfg.tzOffset};
            break;
        case 6: // USA
            tzDst = {Second, Sun, Mar, 2, clockCfg.dsOffset};
            tzStd = {First, Sun, Nov, 2, clockCfg.tzOffset};
            break;
        case 7: // Cuba
            tzDst = {Second, Sun, Mar, 0, clockCfg.dsOffset};
            tzStd = {First, Sun, Nov, 1, clockCfg.tzOffset};
            break;
        case 8: // Mexico
            tzDst = {First, Sun, Apr, 2, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 2, clockCfg.tzOffset};
            break;
        case 9: // Australia (not Austria!)
            tzDst = {First, Sun, Oct, 2, clockCfg.dsOffset};
            tzStd = {First, Sun, Apr, 3, clockCfg.tzOffset};
            break;
        case 10: // New Zealand
            tzDst = {Last, Sun, Sep, 2, clockCfg.dsOffset};
            tzStd = {First, Sun, Apr, 3, clockCfg.tzOffset};
            break;
        case 11: // Samoa
            tzDst = {Last, Sun, Sep, 3, clockCfg.dsOffset};
            tzStd = {First, Sun, Apr, 4, clockCfg.tzOffset};
            break;
        case 12: // Chile
            tzDst = {Second, Sun, Aug, 0, clockCfg.dsOffset};
            tzStd = {Second, Sun, May, 0, clockCfg.tzOffset};
            break;
        case 13: // Easter Island - Chile
            tzDst = {Second, Sun, Aug, 22, clockCfg.dsOffset};
            tzStd = {Second, Sun, May, 22, clockCfg.tzOffset};
            break;
        case 14: // Paraguay
            tzDst = {First, Sun, Oct, 0, clockCfg.dsOffset};
            tzStd = {First, Sun, Mar, 0, clockCfg.tzOffset};
            break;
        case 15: // Fiji, Tonga
            tzDst = {First, Sun, Nov, 2, clockCfg.dsOffset};
            tzStd = {Third, Sun, Jan, 3, clockCfg.tzOffset};
            break;
        case 16: // Lebanon
            tzDst = {Last, Sun, Mar, 0, clockCfg.dsOffset};
            tzStd = {Last, Sun, Oct, 0, clockCfg.tzOffset};
            break;
        case 17: // Jordan
            tzDst = {Last, Thu, Feb, 0, clockCfg.dsOffset};
            tzStd = {Last, Fri, Oct, 1, clockCfg.tzOffset};
            break;
        case 18: // Palestine
            tzDst = {Last, Sat, Mar, 1, clockCfg.dsOffset};
            tzStd = {Last, Sat, Oct, 1, clockCfg.tzOffset};
            break;
        case 19: // Syria
            tzDst = {Last, Fri, Mar, 0, clockCfg.dsOffset};
            tzStd = {Last, Fri, Oct, 0, clockCfg.tzOffset};
            break;
        default:
            // no daylight saving
            tzDst = {Last, Sun, Mar, 1, clockCfg.tzOffset};
            tzStd = {Last, Sun, Oct, 1, clockCfg.tzOffset}; //No Daylight saving offset difference
    }
}

static void printDate(void) {
    Timezone utcTz;
    time_t utc = now();
    //time_t local = tz.toLocal(utc, &tcr);
    int s = second(utc);
    int h = hour(utc);
    int mi = minute(utc);
    int y = year(utc);  
    int mo = month(utc);
    int d = day(utc);

    Serial.printf("UTC YYYY/MM/DD : %d/%d/%d %02d:%02d:%02d\r\n", y, mo, d, h, mi, s);

}

//YYYYMMDDHHMi
static void setDate(char* l) {
    char buf[8] = {0};
    int len = strlen(l);
    int i;
    int y, mo, d, h, mi;
        
    for (i = 0; i < len; i++) {
        if (l[i] < '0' || l[i] > '9') {
            len =-1;
        }
    }

    if (len != 12) {
        Serial.println("The date is malformed. Use: date YyyyMoDaHrMi with no space.");
        Serial.println("Example: 'date 202203101805' for tenth of March 2022, 6:05 PM.");
        return;
    }

    charCopy(buf, l, 4);
    y = strtol(buf, NULL, 0);

    charCopy(buf, l + 4, 2);
    mo = strtol(buf, NULL, 0);

    charCopy(buf, l + 6, 2);
    d = strtol(buf, NULL, 0);

    charCopy(buf, l + 8, 2);
    h = strtol(buf, NULL, 0);

    charCopy(buf, l + 10, 2);
    mi = strtol(buf, NULL, 0);

    setTime(h, mi, 0, d, mo, y);
    printDate();
}

static char setTimeZone(void)
{
  if(!getLocalTime(&tim)){
    Serial.println("Failed to obtain time");
    setTime(0, 10, 0, 1, 1, 2000);
    // dummy timezone rules
    tzDst = {Last, Sun, Mar, 1, 0};
    tzStd = {Last, Sun, Oct, 2, 0};
    tz.setRules(tzDst, tzStd);
    return 1;
  }
  Serial.printf("%d/%d/%d %02d:%02d:%02d, %d, %d\r\n", tim.tm_mday,tim.tm_mon + 1, tim.tm_year + 1900, tim.tm_hour, tim.tm_min, tim.tm_sec, tim.tm_wday, tim.tm_yday);
  setTime(tim.tm_hour, tim.tm_min, tim.tm_sec, tim.tm_mday, tim.tm_mon + 1, tim.tm_year + 1900);

  setDsRules();
  tz.setRules(tzDst, tzStd);
  doNtp = 0; //NTP was now done, reset the counter
  return 0;
}

static char connectToNtp() {
    char r;

    if (!strcmp("#unset#", clockCfg.ntpSsid)) {
        Serial.printf("NTP SSID is unset - skipping connection to NTP\r\n");
        return 100;
    } else {
        Serial.printf("Connecting to AP (%s) ", clockCfg.ntpSsid);
        WiFi.begin(clockCfg.ntpSsid, clockCfg.ntpPass);
        r = 50;
        while (1) {
          wl_status_t stat = WiFi.status();
          //safety check - too many tries
          if (--r == 0) {
            Serial.println(" timed out");
            return 2;
          }
          if (stat== WL_CONNECT_FAILED) {
            Serial.println(" FAILED");
            return 2;
          }
          if (stat == WL_CONNECTED) {
            break;
          }
          delay(500);
          Serial.print(".");
        }
        Serial.println(" CONNECTED");
      
        //init and get UTC time
        Serial.printf("getting current time from: %s\r\n", clockCfg.ntpHost);
        delay(1000);
        configTime(0, 0, clockCfg.ntpHost);
        delay(500);
        r = setTimeZone();

        //try again
        if (r) {
            delay(1000);
            configTime(0, 0, clockCfg.ntpHost);
            delay(500);
            r = setTimeZone();
        }
    
        //disconnect WiFi as it's no longer needed
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
    return r;
}





static void getStatusText(char* response) {
    uint32_t proc;

    sprintf(response, "not implemented");
}




static void setDefaultConfig(void) {
    memset(&clockCfg, 0, sizeof(clockCfg));

    clockCfg.version = 1;

    // WiFi login for entering AP mode and clock configuration
    sprintf(clockCfg.srvSsid, "tickled-%04x", randSeed);
    sprintf(clockCfg.srvPass, "clock-%04x", randSeed);

    // WiFi login to local network. Allows time synchronisation.
    strcpy(clockCfg.ntpSsid, "#unset#");
    strcpy(clockCfg.ntpPass, "#unset#");
    strcpy(clockCfg.ntpHost, "pool.ntp.org");
    clockCfg.colorScheme = 2;
    clockCfg.ntpRate = NTP_RATE_EVERY_DAY;
    clockCfg.minuteStyle = MIN_STYLE_DOT;
    clockCfg.secondStyle = SEC_STYLE_FLOATING;
    clockCfg.oRingStyle = ORING_STYLE_12_DOTS;
    clockCfg.handAnimStyle = HAND_ANIM_FADE;
    clockCfg.brightness = BRIGHNESS_80;
    clockCfg.buttonShort = BUTTON_BRIGHTNESS;
    clockCfg.buttonMedium = BUTTON_COLOR_SCHEME;
}

static void readClockConfig(void) {
    size_t len = sizeof(clockCfg);
    if (!storeLoadRecord("config", (uint8_t*) &clockCfg, &len)) {
        setDefaultConfig();
    }
}



// the setup function runs once when you press reset or power the board
void setup() {
    uint8_t i;
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BOOT, INPUT);

    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_ACM_EN, OUTPUT);

    CLR_IO_PA(M_DATA);
    CLR_IO_PA(M_BUZZER);
    CLR_IO_PA(M_ACM_EN);
    
    
    // Ensure USB CDC On Boot is Enabled (in Arduino IDE, Menu->Tools->Board_options list)  to view serial log on /dev/ttyACM0
    Serial.begin (115200);
    
    delay(2500);
    CLR_IO_PA(1);
    Serial.printf("tick-led %s\r\n", APP_VERSION);
    lineIndex = 0;
    initNetwork();

    {
        uint32_t r;
        byte mac[6];
        WiFi.macAddress(mac);
        randSeed = mac[4];
        randSeed <<= 8;
        randSeed |= mac[5];
        r = acx;
        r += acy;
        r += acz;
        Serial.printf("randSeed=0x%04x r=d\r\n", randSeed, r);
        randomSeed(r + randSeed);
    }

    
    //Clock config
    //memset(&clockCfg, 0, sizeof(clockCfg));
    setDefaultConfig();
    if (storeInit()) {
        Serial.println("Store init failed");
    } else {
        Serial.printf("Store init OK v=%x c=%x \r\n", clockCfg.version, clockCfg.srvSsid[0]);
        readClockConfig();
        if (clockCfg.version == 0 || clockCfg.srvSsid[0] == 0) {
            setDefaultConfig();
        }
    }

    Wire.begin(PIN_SDA, PIN_SCL);
    
    colorSchemeOverride = clockCfg.colorScheme;
    forceHandAnim = 0;

    // turn the buzzer off
    rttl.state = RTTL_IDLE;
    
    acType = 0;
    //detect the accelerometer type
    detectAcm();

    //update accelerometer to average the initial values
    for (i = 0; i < 30; i++) {
        updateAcm();
    }
    acInMotion = 0;
    manualUndim = 0;

#if 0
#if 0
    clockCfg.alarms[0].wday = 10; //every day
    clockCfg.alarms[0].hour = 0;
    clockCfg.alarms[0].minute = 0;
    clockCfg.alarms[0].type = 2;
    clockCfg.alarms[0].loops = 100;

    clockCfg.alarms[1].wday = 10; //every day
    clockCfg.alarms[1].hour = 0;
    clockCfg.alarms[1].minute = 2;
    clockCfg.alarms[1].type = 1;
    clockCfg.alarms[1].loops = 100;
#endif

    clockCfg.tzOffset = 0; //time zone offset in minutes
    clockCfg.dsOffset = 60; //daylight saving offset in minutes
    clockCfg.dsRule = 3;  //daylight saving switch rule

    clockCfg.dimType = 3;
    clockCfg.dimFrom = 22;
    clockCfg.dimFromMin = 30;
    clockCfg.dimTo = 6;
    clockCfg.dimToMin = 15;
#endif    
}


static char hexVal[16] = { '0', '1', '2', '3','4','5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void printHex(uint8_t v) {
  char b[3];
  b[0] = hexVal[v >> 4];
  b[1] = hexVal[v & 0xF];
  b[2] = 0;
  Serial.write(b);
  
}



void printDateTime(time_t t, const char *tz) {
    char buf[32];
    sprintf(buf, "%.2d:%.2d:%.2d %.2d %.2d %d %s",
        hour(t), minute(t), second(t), day(t), month(t), year(t), tz);
    Serial.println(buf);
}

//returns:
//  0: no dimming,  
//  1: dim and show the clock,
//  2: dim but do not show the clock
static uint8_t isDimTime(int hour, int minute) {
    uint16_t dFrom = clockCfg.dimFrom;
    uint16_t dTo = clockCfg.dimTo;

    if (clockCfg.dimType != DIM_TYPE_NONE) {
        uint16_t tim = hour * 60 + minute;

        dFrom *= 60;
        dFrom += clockCfg.dimFromMin;
    
        dTo *= 60;
        dTo += clockCfg.dimToMin;

        //crossing midnight (ie. 22 - 6)
        if (dTo < dFrom) {
            if (tim < dFrom && tim >= dTo) {
                return 0;
            }
        } else
        // not crossing midnight (ie. 1 - 7)
        {
            if (tim < dFrom || tim >= dTo) {
                return 0;    
            }       
        }
        return (clockCfg.dimType >= DIM_TYPE_DARK_DOT) ? 2 : 1;
    }
    return 0;
}

static void ledsSetDimming(int hour, int minute) {
    int i;
    int dimAlpha = -1;
    
    //no dimming in progress
    if (!isDimTime(hour, minute)) {
        goto dimAndBrightness;
    }

    if (acInMotion > 5) {
        manualUndim = 1200;    // ~1 minute
    } else if (manualUndim) {
        //Decrease the temporary undim counter.
        //Once it is back to 0 we dim the clock face.
        manualUndim--;
    }

    if (clockCfg.dimType == DIM_TYPE_DARK) {
        if (manualUndim == 0) {
            ledsClear(0);
            return;
        }
    }
    
    if (clockCfg.dimType == DIM_TYPE_DARK_DOT) {
        if (manualUndim == 0) {
            ledsClear(1);
            // dim the central led 
            ledBlend(1, 0,0,0, 0xDB);
            return;
        }
    }

    //temporary undim is in progress - show the clock in low brightness
    if (manualUndim) {
        dimAlpha = 0xE0; //very low brightnes
        goto dimTheLeds;
    }

    dimAlpha = (clockCfg.dimType == DIM_TYPE_LOW_BR) ? 0xD8 : 0xA0;

dimAndBrightness:
    // when no dimming is taking place then apply brightness
    if (dimAlpha == -1) {
        switch (clockCfg.brightness) {
            case BRIGHNESS_100: return;
            case BRIGHNESS_80: dimAlpha = 0x50; break;
            case BRIGHNESS_60: dimAlpha = 0x90; break;
            case BRIGHNESS_40: dimAlpha = 0xB0; break;
            default: dimAlpha = 0xD0; //20%
        }
    }

dimTheLeds:
    for (i = 1; i <= MAX_RGB; i++) {
        ledBlend(i, 0,0,0, dimAlpha);
    }
}

static void animateMinute(int prevMin, int m, int n, uint32_t color, int anim, int animMax) {
    int dotCount = 7;
    uint8_t* blinds = NULL;
    uint8_t blindCount = 0;
    
    if (anim >= 0) {
        if (clockCfg.handAnimStyle == HAND_ANIM_FADE) {
            int am2 = animMax >> 1;
            int fade;
            
            //hide at the old position
            if (anim < am2) {
                if (prevMin == -1) {
                    fade = 255;
                } else {
                    fade = (anim * 255) / (am2 - 1);
                    m = prevMin;
                    n = 4;
                }
            } 
            //show on a new position
            else {
                fade = 255 - ((anim - am2) * 255 / am2);
            }
            color = colorBlend(color, 0, fade);
        } else
        if (clockCfg.handAnimStyle == HAND_ANIM_GROW) {
            int am2 = animMax >> 1;
            
            //retract at the old position
            if (anim < am2) {
                if (prevMin == -1) {
                    dotCount = 0;
                } else {
                    dotCount = dotCount  - ((anim * dotCount) / (am2 - 1));
                    m = prevMin;
                }
            } 
            //show on a new position
            else {
                dotCount = ((anim - am2) * dotCount) / am2;
            }
        } else
        if (clockCfg.handAnimStyle == HAND_ANIM_BUBBLE) {
            int am2 = animMax >> 1;
            //hide from the old position
            if (anim < am2) {
                if (prevMin == -1) {
                    dotCount = 0;
                } else {
                    blindCount = ((anim * dotCount) / (am2 - 1)) + 1;
                    m = prevMin;
                    blinds = BUBBLES1 + m;
                }
            } 
            //show on a new position
            else {
                blindCount = dotCount - (((anim - am2) * dotCount) / am2) + 1;
                blinds = BUBBLES1 + m;
            }            
        }
    }
    
    ledsSetSpoke(m, color, dotCount, blinds, blindCount);
    //extra minutes
    if (n) {
        ledsSetSpokeMin(m, n, color, 7);
    }
    
}

static void animateHour(int prevHour, int h, uint32_t color, int anim, int animMax) {
    int dotCount = 5;
    uint8_t* blinds = NULL;
    uint8_t blindCount = 0;
    
    if (anim >= 0) {
        if (clockCfg.handAnimStyle == HAND_ANIM_FADE) {
            int am2 = animMax >> 1;
            int fade;
            
            //hide at the old position
            if (anim < am2) {
                if (prevHour == -1) {
                    fade = 255;
                } else {
                    fade = (anim * 255) / (am2 - 1);
                    h = prevHour;
                }
            } 
            //show on a new position
            else {
                fade = 255 - ((anim - am2) * 255 / am2);
            }
            color = colorBlend(color, 0, fade);
        }  else
        if (clockCfg.handAnimStyle == HAND_ANIM_GROW) {
            int am2 = animMax >> 1;
            int fade;
            
            //retract at the old position
            if (anim < am2) {
                if (prevHour == -1) {
                    dotCount = 0;
                } else {
                    dotCount = dotCount  - ((anim * dotCount) / (am2 - 1));
                    h = prevHour;
                }
            } 
            //show on a new position
            else {
                dotCount = ((anim - am2) * dotCount) / am2;
            }
        } else
        if (clockCfg.handAnimStyle == HAND_ANIM_BUBBLE) {
            int am2 = animMax >> 1;
            //hide from the old position
            if (anim < am2) {
                if (prevHour == -1) {
                    dotCount = 0;
                } else {
                    blindCount = ((anim * dotCount) / (am2 - 1)) + 1;
                    h = prevHour;
                    blinds = BUBBLES2 + h;
                }
            } 
            //show on a new position
            else {
                blindCount = dotCount - (((anim - am2) * dotCount) / am2) + 1;
                blinds = BUBBLES2 + h;
            }            
        }
    }
    ledsSetSpoke(h, color, dotCount, blinds, blindCount);   
}

static void setRandomColors(void) {
    uint32_t c[4];
    static int prevRandom = 0;
    int r;
    int diff;
    uint16_t r2;

    r = random(36);
    diff = r - prevRandom;
    //select a random that is a bit different than the previous one
    while (
        (diff < 5 && diff > -5) ||
        diff > 31 ||
        diff < -31
    )
    {
        r = random(36);
        diff = r - prevRandom;
    }
    prevRandom = r;
    r2 = random(8);

    Serial.printf("rand colors: %d,  %d\r\n", r, r2);
    
    getColorPalette(r, c, r2);
    randomColors.outerRing = colorBlend(c[1], 0, 0xC0); //darker shade
    randomColors.hour = c[2];
    randomColors.minute = c[3];
    randomColors.second = c[0];
    randomColors.centerR = (c[0] >> 16) & 0xFF;
    randomColors.centerG = (c[0] >> 8) & 0xFF;
    randomColors.centerB = c[0] & 0xFF;    
}

static void blendColorScheme(ColorScheme* r, ColorScheme* a, ColorScheme* b, uint8_t ratio) {
    // ratio is 1 .. 20
    ratio *= 12;
    //ratio = 254 - ratio;
    
    r->outerRing = colorBlend(a->outerRing, b->outerRing, ratio);
    r->hour = colorBlend(a->hour, b->hour, ratio);
    r->minute = colorBlend(a->minute, b->minute, ratio);
    r->second = colorBlend(a->second, b->second, ratio);
    r->centerR = (r->second >> 16) & 0xFF;
    r->centerG = (r->second >> 8) & 0xFF;
    r->centerB = r->second & 0xFF;    
}

static void setLedClock(time_t t, int& getDay, int& getHour, int& getMin) {
    static int secAnim = 0;
    static int prevSec = -1;
    static int hourAnim = 0;
    static int prevHour = -1;
    static int minAnim = 0;
    static int prevMin = -1;  // didided minute  (0 - 11)
    static int prevRMin = -1; //real minute (0 - 59)
    static int8_t prevDay = -1;
    static uint8_t animRandColorChange = 0;
    
    ColorScheme animColors;
    ColorScheme* cs;
    getDay = weekday(t);
    int s = second(t);
    int h = hour(t);
    int m = minute(t);
    int n = m % 5;
    int rm = m;

    getHour = h;
    getMin = m;

    m /= 5;

    // for preview hand animation (via button press)
    if (forceHandAnim) {
        forceHandAnim = 0;
        prevSec = -1;
        prevMin = -1;
        prevHour = -1;
    }

    //seconds indicator animation
    if (prevSec != s) {
        prevSec = s;
        secAnim = 0;
    } else {
        secAnim++;
    }

    // handle randomly changing color schemes
    if(colorSchemeOverride == 0xFF || colorSchemeOverride > MAX_COLOR_SCHEME) {
        cs = &randomColors;
        //first animation frame of the new second
        if (secAnim == 0) {
            if (
                (colorSchemeOverride == COLSCH_RANDOM_1MIN && prevRMin != rm) ||
                (colorSchemeOverride == COLSCH_RANDOM_5MIN && prevMin != m) ||
                (colorSchemeOverride == COLSCH_RANDOM_HOUR && prevHour != h) ||
                (colorSchemeOverride == COLSCH_RANDOM_DAY && prevDay != getDay)
            ) {
                prevRMin = rm;
                blendColors = randomColors; //backup previous colors
                //generate new random colors, but still show the old colors
                setRandomColors();
                cs = &blendColors;
                animRandColorChange = 1;
            } else {
                animRandColorChange = 0;
            }
        }
        // every first second of the minute when the color change is active
        else if (s == 0 && animRandColorChange){
            blendColorScheme(&animColors, &blendColors, &randomColors, secAnim);
            cs = &animColors;
        }
    } else  {
        cs = &colors[colorSchemeOverride];
    }

    prevDay = getDay;
    
    ledsClear(0);
    ledSetSingle(1, cs->centerR, cs->centerG, cs->centerB); //central LED


    //minute indicator animation
    if (prevMin !=m && minAnim < ANIM_HAND_MAX) {
        minAnim++;
    } else {
        prevMin = m;
        minAnim = -1;      
    }

    //hour indicator animation
    if (prevHour != h && hourAnim < ANIM_HAND_MAX) {
        hourAnim++;
    } else {
        prevHour = h;
        hourAnim = -1;
    }


    if (clockCfg.oRingStyle != ORING_STYLE_HIDDEN) {
        ledsSetOuterRing(cs->outerRing);
    }

    animateMinute(prevMin, m, n, cs->minute, minAnim, ANIM_HAND_MAX);

    //hour on top of minutes
    animateHour(prevHour, h, cs->hour, hourAnim, ANIM_HAND_MAX);

    //seconds indicator
    s = s * 20 + secAnim;
    ledsSetSeconds(s, cs->second);

    //dimming & brightness
    ledsSetDimming(h, rm);
    
    ledsApply();   
}

static void handleNtp(int hour, int minute) {
    char r = 1;
    char safety = 5;
  
    while (r) {
        if (hour == -1 || isDimTime(hour, minute) < 2) {
            delay(1);
            ledsClear(0);
            ledsSetInnerRing(0x200020);
            ledsApply();
        }
        
        safety--;
        if (!safety) {
            break;
        }

        r = connectToNtp();
        // no NTP SSID or password - exit now
        if (r == 100) {
            break;
        }

        if (hour == -1 || isDimTime(hour, minute) < 2) {
            delay(1);
            ledsClear(0);
            ledsSetInnerRing(r ? 0x200000 : 0x2000);
            ledsApply();
        }
        delay(1000);
    }
    state = STATE_CLOCK;
    
}

static void handleAlarms(int d, int h, int m) {
    uint8_t i;
    AlarmItem* alarm;

    //Convert the day index from US format (Sun is the 1st day)
    //to the EU format (Mon is the first day).
    d--;
    if (d == 0) {
        d = 7;
    }

    for (i = 0; i < MAX_ALARMS; i++) {
        alarm = &clockCfg.alarms[i];
        if (alarm->wday > 0) { // check the alarm is disabled
            if (
                (d == alarm->wday) ||                     //exact day match
                (alarm->wday == 8 && d <= 5) ||           //Mon-Fri
                (alarm->wday == 9 && d >=6 && d <= 7) ||  //Sat & Sun
                (alarm->wday == 10)                       //every day
                )
            {
                
                if (alarm->hour == h && alarm->minute == m) {
                    //check the alarm has been processed, if not -> sound the alarm
                    if (alarm->state == 0) {
                        uint16_t beepLoop;

                        alarm->state = 1;
                        beepLoop = (alarm->loops == 0) ? 3000 : alarm->loops; // max is ~ 1 hour of beeping
                         startSongIndex(alarm->type, beepLoop);
                    }
                } else {
                    //set the alarm as unprocessed so it is ready for the next time
                    alarm->state = 0;
                }
            }
        }
    }
}

static void handleTimeEvents(int d, int h, int m) {
    static uint8_t timeEventDone = 0;

    //any time during 3 o'clock AM of local time
    //prevent all tick-leds to connect to NTP at the same time by using randSeed as a minute indicator
    //so that the minutes is = randSeed % 32. That is roughly within the first half an hour depending
    //on randSeed value (which is the last mac address byte)
    if (h == 3 &&  m == (randSeed & 0x1F)) {
    //if (h == 0 && m == 2) {
        if (timeEventDone == 0) {
            timeEventDone = 1;
            Serial.printf("NTP check: doNtp=%d rate=%d ...\r\n", doNtp, clockCfg.ntpRate);
            //handle NTP
            doNtp++;
            if (
                (doNtp == 1 && clockCfg.ntpRate == NTP_RATE_EVERY_DAY) ||
                (doNtp == 2 && clockCfg.ntpRate == NTP_RATE_EVERY_OTHER_DAY) ||
                (doNtp == 7 && clockCfg.ntpRate == NTP_RATE_ONCE_A_WEEK)
            ){
                doNtp = 0;
                handleNtp(h, m);  
            }
            //ensure 'doNtp' does not increase indefinitely
            else if (clockCfg.ntpRate == NTP_RATE_NEVER) {
                doNtp = 0;
            }
        }
    } else {
        timeEventDone = 0;
    }

    handleAlarms(d, h, m);
    
}

static void readSerialGarbage(void) {
    //discard all incoming data
    while (Serial.available() > 0) {
        Serial.read();
    }   
}

static void startClockConfig(void) {
    delay(100);
    ledsClear(0);
    ledsSetWedge(0x20); //blue , low intensity
    ledsApply();
    setupWifi();
    readSerialGarbage();
    clockConfigCounter = CLOCK_CONFIG_TIMEOUT;
}



static void startBootloader(void) {
    delay(10);
    ledsClear(0);
    ledsSetInnerRing(0x805000);
    ledsApply();
    delay(100);

#if 1
    // PIN_BOOT must be connected to Reset pin on the MCU
    // and Button on PIN 0 must be pressed by the user
    pinMode(PIN_BOOT, OUTPUT);
    CLR_IO_PA(M_BOOT);
#endif
    while(1) {};
}

static void beeperTest(void) {
    charCopy(rttlTune, "x:d=4,o=6,b=250:e,8p", 20);
    startSong(rttlTune, 1);
    
}

// Reads input from the serial terminal and returns the command
// which is the first character of the entered text. 

char readTerminalCommands(void) {
    char c;
    char endOfLine = 0;
  
    while (Serial.available() > 0) {
        c = Serial.read();
        inputLine[lineIndex] = c;
        if (c == '\n' || c == '\r') {
            endOfLine = 1;
        }
        //echo input to output
        else {
            Serial.print(c);
        }
        if (lineIndex >= MAX_INPUT_LINE) {
            lineIndex = 0;
            readSerialGarbage();
            Serial.println();
            Serial.println("Error: line too long.");
        } else {
            lineIndex++;
        }
    }
    if (endOfLine) {
        c = COMMAND_NONE;
        if (lineIndex >= 2) {
            c = inputLine[0];  
        }
        Serial.println();
        inputLine[lineIndex > 0 ? lineIndex-1 : 0] = 0;
        lineIndex = 0;
        return c;
    }
    return COMMAND_NONE; 
}

static void exitConfigState(void) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    forceHandAnim = 1;
    state = STATE_CLOCK;
}

void setBeeperFreq(uint16_t f) {
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = f;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);
}

void setBeeperVolume(uint8_t volume) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, volume);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
}

static void startSong(char* song, uint16_t loops) {
    rttl.song = song; //set the song to play
    rttl.songPos = 0; //start the song from the begining
    rttl.octave = 0;  //no octave transposition
    rttl.timeSlice = 4; // 4 ms of time slice
    rttl.loops = loops; // number of repetitions
    rttl.state = RTTL_PLAYING; //playback will start ASAP when playRttl() is called
}

static void startSongIndex(uint8_t songIndex, uint16_t loops) {
    switch (songIndex) {
        case 0: charCopy(rttlTune, "x:d=4,o=7,b=150:e,p", 20); break;
        case 1: charCopy(rttlTune, "x:d=4,o=7,b=250:e,8p,e,2p", 26); break;
        case 2: charCopy(rttlTune, "x:d=8,o=7,b=250:e,p,e,p,e,p,e,2p", 32); break;
        case 3: charCopy(rttlTune, rttl_tune_ateam, sizeof(rttl_tune_ateam) ); break;
        case 4: charCopy(rttlTune, rttl_tune_richw, sizeof(rttl_tune_richw) ); break;
        case 5: charCopy(rttlTune, rttl_tune_indyj, sizeof(rttl_tune_indyj) ); break;
        case 6: charCopy(rttlTune, rttl_tune_swars, sizeof(rttl_tune_swars) ); break;
        case 7: charCopy(rttlTune, rttl_tune_adams, sizeof(rttl_tune_adams) ); break;
        case 8: charCopy(rttlTune, rttl_tune_bod, sizeof(rttl_tune_bod) ); break;
        case 9: charCopy(rttlTune, rttl_tune_dbanj, sizeof(rttl_tune_dbanj) ); break;
        case 10: charCopy(rttlTune, rttl_tune_enter, sizeof(rttl_tune_enter) ); break;
        case 11: charCopy(rttlTune, rttl_tune_gbust, sizeof(rttl_tune_gbust) ); break;
        case 12: charCopy(rttlTune, rttl_tune_jbels, sizeof(rttl_tune_jbels) ); break;
        case 13: charCopy(rttlTune, rttl_tune_kridr, sizeof(rttl_tune_kridr) ); break;
        case 14: charCopy(rttlTune, rttl_tune_12day, sizeof(rttl_tune_12day) ); break;
        case 15: charCopy(rttlTune, rttl_tune_popcr, sizeof(rttl_tune_popcr) ); break;
        case 16: charCopy(rttlTune, rttl_tune_ymca, sizeof(rttl_tune_ymca) ); break;

        default:
            charCopy(rttlTune, "x:d=4,o=7,b=150:e,p", 20); break; //basic beep
    }
    //for basic beeps change the octave (the sound pitch)
    if (songIndex <= 2) {
        rttlTune[8] = '5' + clockCfg.alarmPitch;
    }   
    startSong(rttlTune, loops);
}

static void handleTerminalCommand() {
    //Serial.printf ("terminal command: '%s'\r\n", inputLine);

    if (strstr(inputLine, "tune ") == inputLine) {
        uint8_t index = (uint8_t) strtol(inputLine + 5, NULL, 0);
        startSongIndex(index, 2);
        while (rttl.state == RTTL_PLAYING) {
            playRttl(&rttl, true); // true: auto-delay of 'timeSlice' ms, false: delay of 'time-slice' must be performed here
        } 
    } else
    if (!strcmp("beep", inputLine)) {
        startSongIndex(0, 1);
        while (rttl.state == RTTL_PLAYING) {
            playRttl(&rttl, true); // true: auto-delay of 'timeSlice' ms, false: delay of 'time-slice' must be performed here
        } 
    } else
    if (!strcmp("help", inputLine) || !strcmp("?", inputLine)) {
        Serial.println("ver       : prints the FW version");
        Serial.println("exit      : exits configuration back to clock mode");
        Serial.println("reboot    : reboots tick-led");
        Serial.println("beep      : tests the beeper");
        Serial.println("tune X    : plays a tune X (0-16)");              
        Serial.println("print     : prints current config");
        Serial.println("print key : prints config key options");
        Serial.println("key=value : sets config key to value. Use 'print' to list config keys");
        Serial.println("resetconf : reset config values to defaults (Forgets WiFi NTP password)");
        Serial.println("saveconf  : saves current config");
        Serial.println("defconf   : sets and saves default config");
        Serial.println("ntp       : connects to NTP server - time sync");
        Serial.println("date      : prints current date and time");
        Serial.println("date YYYYMMDDHHMM : sets UTC time YearMonthDay HoursMinutes");
        Serial.println("wifi stop : stops the wifi server");
        Serial.println("help      : shows this help text");
    } else
    if (!strcmp("ver", inputLine)) {
        Serial.printf("tick-led %s\r\n", APP_VERSION);
    } else
    if (!strcmp("ntp", inputLine)) {
        exitConfigState();       
        handleNtp(-1, -1);
    } else   
    if (strstr(inputLine, "print ") == inputLine) {
        printConfigParam(inputLine + 6);
    } else
    if (!strcmp("print", inputLine)) {
        printConfig();
    } else
    if (strstr(inputLine, "date ") == inputLine) {
        setDate(inputLine + 5);
    } else
    if (!strcmp("date", inputLine)) {
        printDate();
    } else
    if (!strcmp("exit", inputLine)) {
        exitConfigState();
    } else
    if (!strcmp("wifi stop", inputLine)) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Serial.println("WiFi AP is now stopped");
    } else
    if (!strcmp("reboot", inputLine)) {
        pinMode(PIN_BOOT, OUTPUT);
        CLR_IO_PA(M_BOOT);
    } else
    if (!strcmp("defconf", inputLine)) {
        bool res;
        setDefaultConfig();
        res = storeSaveRecord("config", (uint8_t*) &clockCfg, sizeof(ClockConfig));
        Serial.printf("Config defaults save: %s\r\n", res ? "OK":"Failed");       
    } else
    if (!strcmp("saveconf", inputLine)) {
        bool res = storeSaveRecord("config", (uint8_t*) &clockCfg, sizeof(ClockConfig));
        Serial.printf("Config save: %s\r\n", res ? "OK":"Failed");       
    } else
    if (!setConfigParam(inputLine)) {
        Serial.printf("%s now set\r\n", inputLine);
    } else {
        Serial.printf("unknown command: '%s'. Use '?' or 'help' for command list\r\n", inputLine);
    }
}

static void handleButtonPress(uint8_t medium) {
    static uint8_t randCol = 0;
    uint8_t action = medium ? clockCfg.buttonMedium : clockCfg.buttonShort;

    switch (action) {
        case BUTTON_BRIGHTNESS:
            clockCfg.brightness++;
            if (clockCfg.brightness > 4) {
                clockCfg.brightness = 0;
            }
            break;
        case BUTTON_COLOR_SCHEME:
            if (randCol == 0) {
                colorSchemeOverride++;
                if (colorSchemeOverride > MAX_COLOR_SCHEME) {
                    randCol = 1;
                }
            }
            if (randCol) {
                setRandomColors();
                colorSchemeOverride = 0xFF;  //custom random colors
                randCol++;

                //after 5 random colors go back to predefined
                if (randCol > 5) {
                    randCol = 0;
                    colorSchemeOverride = 0;                   
                }
            }
            break;

        case BUTTON_HAND_ANIM:
            clockCfg.handAnimStyle++;
            if (clockCfg.handAnimStyle > 3) {
                clockCfg.handAnimStyle = 0;
            }
            forceHandAnim = 1;
            break;
    }

    //test dimming modes
    #if 0
    clockCfg.dimType++;
    if (clockCfg.dimType > 4) {
        clockCfg.dimType = 0;
    }
    Serial.printf("dim=%d\r\n", clockCfg.dimType);
    #endif
}

static void updateAcmMC3479(void) {
    int16_t x, y, z;

    //assume the ACM is awake and set-up by the detectAcm()
    Wire.beginTransmission(ACM_MC3479_ADDR);
    Wire.write(byte(0xD)); //X,Y,Z
    Wire.endTransmission();
    Wire.requestFrom(ACM_MC3479_ADDR, 6);
    if (Wire.available() == 6) {
        uint8_t motion;
        // backup previous value
        x = acx;
        y = acy;
        z = acz;

        // average the previous value with the current value
        // to smooth out the fuzzy accelerometer values
        // normalize the Z to 1g = 254

        acx += ((int16_t) ((uint16_t) Wire.read() | (((uint16_t)Wire.read()) << 8))) /64;
        acx /= 2;
        acy += ((int16_t) ((uint16_t) Wire.read() | (((uint16_t)Wire.read()) << 8))) /64;
        acy /= 2;
        acz += ((int16_t) ((uint16_t) Wire.read() | (((uint16_t)Wire.read()) << 8))) /64;
        acz /= 2;

        // compute motion indicator
        motion = (x - acx > MOTION_THRESHOLD) || (acx - x > MOTION_THRESHOLD) ||
                    (y - acy > MOTION_THRESHOLD) || (acy - y > MOTION_THRESHOLD) ||
                    (z - acz > MOTION_THRESHOLD) || (acz - z > MOTION_THRESHOLD);
        //accumulate motion events
        if (motion) {
            if (acInMotion < 20) acInMotion++;
        } else {
            if (acInMotion > 0) acInMotion--;
        }
#if ACM_INFO
        if(acInMotion) {
            Serial.printf("xyz: %i, %i, %i cnt %d:\r\n", (int)acx, (int)acy, (int)acz, acInMotion);
        }
#endif        
    }
}


static void updateAcmMMA8491Q(void) {
    static int deb = 0;
    SET_IO_PA(M_ACM_EN); //enable accmeter - it wakes up
    delay(1); //the delay is required to allow accmeter to sample the values
    Wire.beginTransmission(ACM_MMA8491Q_ADDR);
    Wire.write(byte(0x00)); //status reg  
    Wire.endTransmission();
    Wire.requestFrom(ACM_MMA8491Q_ADDR, 7);

    if (Wire.available() == 7) {
        uint8_t motion;
        int16_t x, y, z;

        x = Wire.read(); //discard byte 0

        // backup previous value
        x = acx;
        y = acy;
        z = acz;

        // average the previous value with the current value
        // to smooth out the fuzzy accelerometer values
        // normalize the 1g = 254
        acx += ((int16_t) ((((uint16_t)Wire.read()) << 8) | Wire.read())) /16;
        acx /= 2;
        acy += ((int16_t) ((((uint16_t)Wire.read()) << 8) | Wire.read())) /16;
        acy /= 2;
        acz += ((int16_t) ((((uint16_t)Wire.read()) << 8) | Wire.read())) /16;
        acz /= 2;
        
        // compute motion indicator
        motion = (x - acx > MOTION_THRESHOLD) || (acx - x > MOTION_THRESHOLD) ||
                    (y - acy > MOTION_THRESHOLD) || (acy - y > MOTION_THRESHOLD) ||
                    (z - acz > MOTION_THRESHOLD) || (acz - z > MOTION_THRESHOLD);
        //accumulate motion events
        if (motion) {
            if (acInMotion < 20) acInMotion++;
        } else {
            if (acInMotion > 0) acInMotion--;
        }
#if 1        
        if(acInMotion) {
            Serial.printf("xyz: %i, %i, %i cnt %d:\r\n", (int)acx, (int)acy, (int)acz, acInMotion);
        }
#endif        
    }
    
    CLR_IO_PA(M_ACM_EN); //disable accmeter
}

static void updateAcm(void) {
    switch (acType) {
        case ACM_MMA8491Q:
            updateAcmMMA8491Q();
            break;
        case ACM_MC3479:
            updateAcmMC3479();
            break;
    }
}

static void detectAcm(void) {
    acType = ACM_NONE;
    
    //check MMA8491Q
    {
        SET_IO_PA(M_ACM_EN); //enable accmeter - it wakes up
        delay(1); //the delay is required to allow accmeter to sample the values
        Wire.beginTransmission(ACM_MMA8491Q_ADDR);
        Wire.write(byte(0x00)); //status reg  
        Wire.endTransmission();
        Wire.requestFrom(ACM_MMA8491Q_ADDR, 1); //request status
        if (Wire.available() == 1) {
            uint8_t id = Wire.read();
            Serial.printf("acm id=0x%x\r\n", id);
            if (id == 0xf) {
                acType = ACM_MMA8491Q;          
            }
        }
        CLR_IO_PA(M_ACM_EN); //disable accmeter
    }

    // check ACM_MC3479
    if (acType == ACM_NONE) {
        Wire.beginTransmission(ACM_MC3479_ADDR);
        Wire.write(byte(0x18)); //chip id
        Wire.endTransmission();
        Wire.requestFrom(ACM_MC3479_ADDR, 1);

        if (Wire.available() == 1) {
            uint8_t id = Wire.read();
            Serial.printf("acm id=0x%x\r\n", id);
            if (id = 0xa4) {
                acType = ACM_MC3479;
                Wire.beginTransmission(ACM_MC3479_ADDR);
                Wire.write(byte(0x7)); //set mode
                Wire.write(byte(0x1)); //wake up
                Wire.endTransmission();
            }
        }
    }
    Serial.printf("acm type=%d\r\n", acType);
}


static void setupBuzzerTimer(void) {

    ledc_channel_config_t ledc_channel;
    ledc_timer_config_t ledc_timer;
    
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = 4186;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel.gpio_num   = 5;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_3;
    ledc_channel.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;
    ledc_channel_config(&ledc_channel);
}


void loop() {
  static uint8_t buttonWasReleased = 1;


  switch (state) {
    case STATE_INIT:
        state = STATE_NTP;
        setupBuzzerTimer();
        //state = STATE_CLOCK; //test - skip ntp
        break;
    case STATE_NTP:
        handleNtp(-1, -1);
        break;
    case STATE_CONFIG:
        {
            handleWifi();
            //check button
            if (!(READ_IO_PA(M_BUTTON))) {
                buttonPress ++;
                //very long press (~8 seconds) during config mode -> upgrade reboot boot
                if (buttonPress > 500) {
                    buttonPress = 0;
                    startBootloader();
                    state = 0xFF;
                }
            } else {
                //short press during config mode -> return to clock mode
                if (buttonPress > 3 && buttonPress < 40 && buttonWasReleased) {
                    exitConfigState();
                }
                buttonPress = 0;
                buttonWasReleased = 1;

                //auto exit after inacativity
                clockConfigCounter--;
                if (clockConfigCounter == 0) {
                    exitConfigState();
                    break;
                }
            }
            if (COMMAND_NONE != readTerminalCommands()) {
                handleTerminalCommand();
            }
        }
        break;
    case STATE_CONFIG_EXIT: {
            handleWifi();
            if (configExit == 0) {
                configExit = 10;
                state = STATE_NTP;
            } else {
                configExit--;
            }
        }
        break;
    case STATE_CLOCK:
        {    
            //unsigned long currentMillis = xTaskGetTickCount();
            TimeChangeRule *tcr;
            time_t utc = now();
            time_t local = tz.toLocal(utc, &tcr);
            int hour, minute, day;
            // display LEDs and get time
            setLedClock(local, day, hour, minute);
            handleTimeEvents(day, hour, minute);

            //check button
            //button is pressed
            if (!(READ_IO_PA(M_BUTTON))) {
                buttonPress ++;
                if (buttonPress > 50) {
                    buttonPress = 0;
                    startClockConfig();
                    buttonWasReleased = 0;
                    state = STATE_CONFIG;
                }
            } else 
            //button is released
            {
                //check for how long the button was pressed
                //short button press
                if (buttonPress >= 1 && buttonPress <= 15) {
                    beeperTest();
                    handleButtonPress(0);
                } 
                else if (buttonPress > 15 && buttonPress <=30) {
                    handleButtonPress(1); 
                }
                buttonPress = 0;
            }

            if (rttl.state != RTTL_IDLE) {
                uint8_t p = 10;
                //currentMillis = xTaskGetTickCount();
                delay(1); //LEDs update takes ~ 3 ms
                playRttl(&rttl, false);
                updateAcm();
                //mute the beeper when sustained motion is detected
                if (acInMotion > 5) {
                    stopRttl(&rttl);
                }
                delay(rttl.timeSlice - 1); //acm update takes ~ 1ms
                while(p--) {
                    playRttl(&rttl, false); //4ms
                    delay(rttl.timeSlice);
                }
                playRttl(&rttl, false); //4ms
                //the next rttl delay will happen during LED redraw
                //all together total wait time is ~ 46 ms
            } else { 
                updateAcm();
                delay(46); //~20 fps (4 mS to set the leds: 1.4 uS per bit * 24 bits per color * 134 LEDs )
            }
        }break;
  }
}
