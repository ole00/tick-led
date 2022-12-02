#ifndef _TICKLED_H_
#define _TICKLED_H_


#define APP_VERSION "ver. 0.9"

// pin config for WEMOS S2 mini board
#define PIN_BUTTON 0
#define PIN_DATA 16
#define PIN_BOOT 14
#define PIN_BUZZER 5

#define PIN_ACM_EN 18
#define PIN_SDA 33
#define PIN_SCL 35

#define M_BUTTON (uint32_t) 1 << PIN_BUTTON
#define M_DATA (uint32_t) 1 << PIN_DATA
#define M_BOOT (uint32_t) 1 << PIN_BOOT
#define M_BUZZER (uint32_t) 1 << PIN_BUZZER
#define M_ACM_EN (uint32_t) 1 << PIN_ACM_EN


// quick GPIO functions for ESP32-S2

// X = Pins 0 - 31
#define IO_PA(X,V) if (V) { GPIO.out_w1ts = (X);} else {GPIO.out_w1tc = (X);}
#define SET_IO_PA(X) GPIO.out_w1ts = (X)
#define CLR_IO_PA(X) GPIO.out_w1tc = (X)
#define READ_IO_PA(MASK)  ((GPIO.in) & (MASK))

// X = Pins 0 - 31, but mapped to GPIO 32 - 40
#define SET_IO_PB(X) GPIO.out1_w1ts.val = (X)
#define CLR_IO_PB(X) GPIO.out1_w1tc.val = (X)
#define READ_IO_PB(MASK)  ((GPIO.in1.val) & (MASK))
#define WRITE_IO_PB(X) GPIO.out1.val = (X)

#define INP_IO_PB(X) GPIO.enable1_w1tc.val = (X)
#define OUT_IO_PB(X) GPIO.enable1_w1ts.val = (X)

#define ACM_NONE     0
#define ACM_MMA8491Q 1
#define ACM_MC3479   2

#define MAX_RGB 133
#define MAX_LEDS (3 * MAX_RGB)

#define NTP_RATE_NEVER            0
#define NTP_RATE_EVERY_DAY        1
#define NTP_RATE_EVERY_OTHER_DAY  2
#define NTP_RATE_ONCE_A_WEEK      3


#define MIN_STYLE_HIDDEN          0
#define MIN_STYLE_FULL_BAR        1
#define MIN_STYLE_DOT             2

#define SEC_STYLE_HIDDEN          0
#define SEC_STYLE_DOT             1
#define SEC_STYLE_FLOATING        2

#define ORING_STYLE_HIDDEN        0
#define ORING_STYLE_4_DOTS        1
#define ORING_STYLE_12_DOTS       2

#define DIM_TYPE_NONE             0
#define DIM_TYPE_MEDIUM_BR        1
#define DIM_TYPE_LOW_BR           2
#define DIM_TYPE_DARK_DOT         3
#define DIM_TYPE_DARK             4

#define BRIGHNESS_100             0
#define BRIGHNESS_80              1
#define BRIGHNESS_60              2
#define BRIGHNESS_40              3
#define BRIGHNESS_20              4

#define HAND_ANIM_NONE            0
#define HAND_ANIM_FADE            1
#define HAND_ANIM_GROW            2
#define HAND_ANIM_BUBBLE          3

#define BUTTON_BRIGHTNESS         0
#define BUTTON_COLOR_SCHEME       1
#define BUTTON_HAND_ANIM          2

#define COLSCH_RANDOM_DAY         6
#define COLSCH_RANDOM_HOUR        7
#define COLSCH_RANDOM_5MIN        8
#define COLSCH_RANDOM_1MIN        9


#define STATE_INIT 0
#define STATE_NTP 1
#define STATE_CONFIG 2
#define STATE_CONFIG_EXIT 3
#define STATE_CLOCK 4

#define ANIM_HAND_MAX             14

#define MAX_LANG 2

#define MAX_ALARMS 5

typedef struct AlarmItem {
    uint8_t wday;    //0 - disabled, 1-Sun, 2- Mon etc.
    uint8_t hour;
    uint8_t minute;
    uint8_t type;  //beep pattern
    uint16_t loops; //number of loops
    uint8_t state; //in progress, snoozed, stopped etc.
    uint8_t reserved;
} AlarmItem;

typedef struct ClockConfig {
    uint16_t version;
    uint8_t  reserved1;
    uint8_t  langIndex;  //language index
    char ntpSsid[48];
    char ntpPass[48];
    char ntpHost[48];

    char srvSsid[48];
    char srvPass[48];

    int32_t tzOffset; //time zone offse in minutes
    int32_t dsOffset; //daylight saving offset in minutes
    uint32_t dsRule;  //daylight saving switch rule
    
    uint32_t colorScheme;

    //offset 260
    
    uint8_t ntpRate;
    uint8_t minuteStyle;
    uint8_t secondStyle;
    uint8_t oRingStyle; //outer ring style

    uint8_t dimType;
    uint8_t dimFrom;  //hours
    uint8_t dimFromMin; //minutes
    uint8_t dimTo;   // hours
    uint8_t dimToMin; //minutes
    uint8_t handAnimStyle;
    uint8_t brightness;

    uint8_t buttonShort;
    uint8_t buttonMedium;

    uint8_t reserved2[14];
    //offset 287  

    uint8_t alarmPitch;
    AlarmItem alarms[MAX_ALARMS];
    //offset 312
    uint8_t reserved3[666]; //total size 1024 bytes
} ClockConfig;


static inline void charCopy(char* dst, const char* src, uint16_t len) {
    dst[len] = 0;
    while (len--) {
        *dst = *src;
        dst++;
        src++;
    }
}

void setBeeperFreq(uint16_t f);
void setBeeperVolume(uint8_t volume);

#endif /*_TICKLED_H_ */
