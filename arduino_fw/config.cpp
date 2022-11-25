#include <stdint.h>
#include <string.h>
#include <Arduino.h>

#include "tick-led.h"

#define COPY_STR_VALUE(D,V,M) if(V[0]){int l=strlen(V); charCopy(D, V, (M) > l ? (M) : l);}
#define COPY_INT_VALUE(D,V) if(V[0]) {D = (int) strtol(V, NULL,0);}
#define COPY_BYTE_VALUE(D,V) if(V[0]) {D = (uint8_t) strtol(V, NULL,0);}
#define COPY_SHORT_VALUE(D,V) if(V[0]) {D = (uint16_t) strtol(V, NULL,0);}

extern ClockConfig clockCfg;
extern uint32_t colorSchemeOverride;

uint8_t getAlarmLoopIndex(uint16_t i) {
    switch (i) {
        case 0 ... 3: return i;
        case 4 ... 50: return 4 + (i / 10);
        case 100: return 10;
        case 300: return 11;
    }
    return 0;
}


void printConfig(void)
{
    Serial.printf("cfgssid='%s' : Configuration server SSID\r\n", clockCfg.srvSsid);
    Serial.printf("cfgpwd='%s' : Configuration server password\r\n", clockCfg.srvPass);
    Serial.printf("ntpssid='%s' : Internet SSID\r\n", clockCfg.ntpSsid);
    Serial.printf("ntppwd='**...**' : Internet password\r\n");
    Serial.printf("ntphost=%s : NTP host\r\n", clockCfg.ntpHost);
    Serial.printf("colsch=%d (%d - %d): color scheme\r\n", clockCfg.colorScheme, 0, 5);
    Serial.printf("ntprate=%d  (%d - %d): NTP rate\r\n", clockCfg.ntpRate, 0, 3);

    Serial.printf("tzofs=%d : Time zone offset (min)\r\n", clockCfg.tzOffset);
    Serial.printf("tzods=%d : Daylight saving offset (min)\r\n", clockCfg.dsOffset);
    Serial.printf("tzdsr=%d  (%d - %d): Daylight saving rule\r\n", clockCfg.dsRule, 0, 19);

    Serial.printf("minstl=%d  (%d - %d): Minute hand style\r\n", clockCfg.minuteStyle, 0, 2);
    Serial.printf("secstl=%d  (%d - %d): Seconds indicator style\r\n", clockCfg.secondStyle, 0, 2);

    Serial.printf("oristl=%d  (%d - %d): Outer ring style\r\n", clockCfg.oRingStyle, 0, 2);
    Serial.printf("vedimg=%d  (%d - %d): Dimmig type\r\n", clockCfg.dimType, 0, 4);
    Serial.printf("vedimfh=%d  (%d - %d): Dim from - hour\r\n", clockCfg.dimFrom, 0, 23);
    Serial.printf("vedimfm=%d  (%d - %d): Dim from - minute\r\n", clockCfg.dimFromMin, 0, 59);
    Serial.printf("vedimth=%d  (%d - %d): Dim to - hour\r\n", clockCfg.dimTo, 0, 23);
    Serial.printf("vedimtm=%d  (%d - %d): Dim to - minute\r\n", clockCfg.dimToMin, 0, 23);

    Serial.printf("vehana=%d  (%d - %d): Hand anim style\r\n", clockCfg.handAnimStyle, 0, 3);
    Serial.printf("vebrig=%d  (%d - %d): Brightness\r\n", clockCfg.brightness, 0, 4);

    Serial.printf("bushp=%d  (%d - %d): Short button press action\r\n", clockCfg.buttonShort, 0, 2);
    Serial.printf("bumep=%d  (%d - %d): Medium button press action\r\n", clockCfg.buttonMedium, 0, 2);

    //Alarms
    Serial.printf("alapit=%d  (%d - %d): Alarm pitch\r\n", clockCfg.alarmPitch, 0, 2);
    
    Serial.printf("ala1day=%d  (%d - %d): Alarm 1 day\r\n", clockCfg.alarms[0].wday, 0, 10);
    Serial.printf("ala1hr=%d  (%d - %d): Alarm 1 hour\r\n", clockCfg.alarms[0].hour, 0, 23);
    Serial.printf("ala1min=%d  (%d - %d): Alarm 1 minute\r\n", clockCfg.alarms[0].minute, 0, 59);
    Serial.printf("ala1bep=%d  (%d - %d): Alarm 1 beep type\r\n", clockCfg.alarms[0].type, 0, 2);
    Serial.printf("ala1lop=%d  (%d - %d): Alarm 1 loops\r\n", clockCfg.alarms[0].loops, 0, 10000);

    Serial.printf("ala2day=%d  (%d - %d): Alarm 2 day\r\n", clockCfg.alarms[1].wday, 0, 10);
    Serial.printf("ala2hr=%d  (%d - %d): Alarm 2 hour\r\n", clockCfg.alarms[1].hour, 0, 23);
    Serial.printf("ala2min=%d  (%d - %d): Alarm 2 minute\r\n", clockCfg.alarms[1].minute, 0, 59);
    Serial.printf("ala2bep=%d  (%d - %d): Alarm 2 beep type\r\n", clockCfg.alarms[1].type, 0, 2);
    Serial.printf("ala2lop=%d  (%d - %d): Alarm 2 loops\r\n", clockCfg.alarms[1].loops, 0, 10000);

    Serial.printf("ala3day=%d  (%d - %d): Alarm 3 day\r\n", clockCfg.alarms[2].wday, 0, 10);
    Serial.printf("ala3hr=%d  (%d - %d): Alarm 3 hour\r\n", clockCfg.alarms[2].hour, 0, 23);
    Serial.printf("ala3min=%d  (%d - %d): Alarm 3 minute\r\n", clockCfg.alarms[2].minute, 0, 59);
    Serial.printf("ala3bep=%d  (%d - %d): Alarm 3 beep type\r\n", clockCfg.alarms[2].type, 0, 2);
    Serial.printf("ala3lop=%d  (%d - %d): Alarm 3 loops\r\n", clockCfg.alarms[2].loops, 0, 10000);

    Serial.printf("ala4day=%d  (%d - %d): Alarm 4 day\r\n", clockCfg.alarms[3].wday, 0, 10);
    Serial.printf("ala4hr=%d  (%d - %d): Alarm 4 hour\r\n", clockCfg.alarms[3].hour, 0, 23);
    Serial.printf("ala4min=%d  (%d - %d): Alarm 4 minute\r\n", clockCfg.alarms[3].minute, 0, 59);
    Serial.printf("ala4bep=%d  (%d - %d): Alarm 4 beep type\r\n", clockCfg.alarms[3].type, 0, 2);
    Serial.printf("ala4lop=%d  (%d - %d): Alarm 4 loops\r\n", clockCfg.alarms[3].loops, 0, 10000);

    Serial.printf("ala5day=%d  (%d - %d): Alarm 5 day\r\n", clockCfg.alarms[4].wday, 0, 10);
    Serial.printf("ala5hr=%d  (%d - %d): Alarm 5 hour\r\n", clockCfg.alarms[4].hour, 0, 23);
    Serial.printf("ala5min=%d  (%d - %d): Alarm 5 minute\r\n", clockCfg.alarms[4].minute, 0, 59);
    Serial.printf("ala5bep=%d  (%d - %d): Alarm 5 beep_type\r\n", clockCfg.alarms[4].type, 0, 2);
    Serial.printf("ala5lop=%d  (%d - %d): Alarm 5 loops\r\n", clockCfg.alarms[4].loops, 0, 10000);

}

uint8_t printConfigAlarm(uint8_t i, char* param) {
    if (i > 4 || param == NULL)  {
        return 1;
    }
    if (!strcmp(param, "day")) {
        Serial.printf("Alarm %d day = %d\r\n", i + 1, clockCfg.alarms[i].wday);
        Serial.printf("   0: alarm disabled\r\n");
        Serial.printf("   1: Mon, 2: Tue, 3: Wed, 4: Thu, 5: Fri, 6:Sat, 7: Sun\r\n");
        Serial.printf("   8: Mon - Fri\r\n");
        Serial.printf("   9: Sat - Sun\r\n");
        Serial.printf("   10: Every day\r\n");
    } else
    if (!strcmp(param, "hr")) {
        Serial.printf("Alarm %d hour = %d\r\n", i + 1, clockCfg.alarms[i].hour);       
    } else
    if (!strcmp(param, "min")) {
        Serial.printf("Alarm %d minute = %d\r\n", i + 1, clockCfg.alarms[i].minute);       
    }
    if (!strcmp(param, "bep")) {
        Serial.printf("Alarm %d beep type = %d\r\n", i + 1, clockCfg.alarms[i].type);
        Serial.printf("   0: one slow beep\r\n");
        Serial.printf("   1: two beeps\r\n");
        Serial.printf("   2: four quick beeps\r\n");
    } else
    if (!strcmp(param, "lop")) {
        Serial.printf("Alarm %d loops = %d\r\n", i + 1, clockCfg.alarms[i].loops);
        Serial.printf("   0: loops forever until stopped by a tap\r\n");
        Serial.printf("   1..10000: number of loops or when stopped by a tap\r\n");
    }
    else 
    {
        return 1;    
    }
    return 0;
}

uint8_t printConfigParam(char* p) {

    if (p[0] == 'a' && p[1] == 'l' && p[2] == 'a' && p[3] >= '1' &&  p[3] <= '5') {
        return printConfigAlarm(p[3] - '1', p + 4);   
    }
    return 1;
}

uint8_t setConfigParam(const char* param) {
    char* value;

    value = strchr(param, '=');
    if (NULL == value) {
        return 1;
    }
    value[0] = 0;
    value++;
    Serial.printf("apply param %s : %s\r\n", param, value);

    if (!strcmp(param, "lang")) {
        COPY_BYTE_VALUE(clockCfg.langIndex, value);
        if (clockCfg.langIndex >= MAX_LANG) {
            clockCfg.langIndex = 0;
        }
        Serial.printf("language index=%d\r\n", clockCfg.langIndex);
    } else
    if (!strcmp(param, "cfgssid")) {
        COPY_STR_VALUE(clockCfg.srvSsid, value, 47);
    } else
    if (!strcmp(param, "cfgpwd")) {
        COPY_STR_VALUE(clockCfg.srvPass, value, 47);
    } else
    if (!strcmp(param, "ntpssid")) {
        COPY_STR_VALUE(clockCfg.ntpSsid, value, 47);
    } else
    if (!strcmp(param, "ntppwd")) {
        COPY_STR_VALUE(clockCfg.ntpPass, value, 47);
    } else
    if (!strcmp(param, "ntphost")) {
        COPY_STR_VALUE(clockCfg.ntpHost, value, 47);
    } else
    if (!strcmp(param, "ntprate")) {
        COPY_BYTE_VALUE(clockCfg.ntpRate, value);
    } else
    if (!strcmp(param, "colsch")) {
        COPY_INT_VALUE(clockCfg.colorScheme, value);
        colorSchemeOverride = clockCfg.colorScheme;
    } else
    if (!strcmp(param, "tzofs")) {
        COPY_INT_VALUE(clockCfg.tzOffset, value);
    } else
    if (!strcmp(param, "tzods")) {
        COPY_INT_VALUE(clockCfg.dsOffset, value);
    } else
    if (!strcmp(param, "tzdsr")) {
        COPY_INT_VALUE(clockCfg.dsRule, value);
    } else
    if (!strcmp(param, "minstl")) {
        COPY_BYTE_VALUE(clockCfg.minuteStyle, value);
    } else
    if (!strcmp(param, "secstl")) {
        COPY_BYTE_VALUE(clockCfg.secondStyle, value);
    } else
    if (!strcmp(param, "oristl")) {
        COPY_BYTE_VALUE(clockCfg.oRingStyle, value);
    } else
    if (!strcmp(param, "vedimg")) {
        COPY_BYTE_VALUE(clockCfg.dimType, value);
    } else
    if (!strcmp(param, "vedimfh")) {
        COPY_BYTE_VALUE(clockCfg.dimFrom, value);
    } else
    if (!strcmp(param, "vedimfm")) {
        COPY_BYTE_VALUE(clockCfg.dimFromMin, value);
    } else
    if (!strcmp(param, "vedimth")) {
        COPY_BYTE_VALUE(clockCfg.dimTo, value);
    } else
    if (!strcmp(param, "vedimtm")) {
        COPY_BYTE_VALUE(clockCfg.dimToMin, value);
    } else
    if (!strcmp(param, "vehana")) {
        COPY_BYTE_VALUE(clockCfg.handAnimStyle, value);
    } else
    if (!strcmp(param, "vebrig")) {
        COPY_BYTE_VALUE(clockCfg.brightness, value);
    } else
    if (!strcmp(param, "bushp")) {
        COPY_BYTE_VALUE(clockCfg.buttonShort, value);
    } else
    if (!strcmp(param, "bumep")) {
        COPY_BYTE_VALUE(clockCfg.buttonMedium, value);
    } else

    //Alarms
    if (!strcmp(param, "alapit")) {
        COPY_BYTE_VALUE(clockCfg.alarmPitch, value);
    } else
    if (!strcmp(param, "ala1day")) {
        COPY_BYTE_VALUE(clockCfg.alarms[0].wday, value);
    } else
    if (!strcmp(param, "ala1hr")) {
        COPY_BYTE_VALUE(clockCfg.alarms[0].hour, value);
    } else
    if (!strcmp(param, "ala1min")) {
        COPY_BYTE_VALUE(clockCfg.alarms[0].minute, value);
    } else
    if (!strcmp(param, "ala1bep")) {
        COPY_BYTE_VALUE(clockCfg.alarms[0].type, value);
    } else
    if (!strcmp(param, "ala1lop")) {
        COPY_BYTE_VALUE(clockCfg.alarms[0].loops, value);
    } else

    if (!strcmp(param, "ala2day")) {
        COPY_BYTE_VALUE(clockCfg.alarms[1].wday, value);
    } else
    if (!strcmp(param, "ala2hr")) {
        COPY_BYTE_VALUE(clockCfg.alarms[1].hour, value);
    } else
    if (!strcmp(param, "ala2min")) {
        COPY_BYTE_VALUE(clockCfg.alarms[1].minute, value);
    } else
    if (!strcmp(param, "ala2bep")) {
        COPY_BYTE_VALUE(clockCfg.alarms[1].type, value);
    } else
    if (!strcmp(param, "ala2lop")) {
        COPY_SHORT_VALUE(clockCfg.alarms[1].loops, value);
    } else

    if (!strcmp(param, "ala3day")) {
        COPY_BYTE_VALUE(clockCfg.alarms[2].wday, value);
    } else
    if (!strcmp(param, "ala3hr")) {
        COPY_BYTE_VALUE(clockCfg.alarms[2].hour, value);
    } else
    if (!strcmp(param, "ala3min")) {
        COPY_BYTE_VALUE(clockCfg.alarms[2].minute, value);
    } else
    if (!strcmp(param, "ala3bep")) {
        COPY_BYTE_VALUE(clockCfg.alarms[2].type, value);
    } else
    if (!strcmp(param, "ala3lop")) {
        COPY_SHORT_VALUE(clockCfg.alarms[2].loops, value);
    } else

    if (!strcmp(param, "ala4day")) {
        COPY_BYTE_VALUE(clockCfg.alarms[3].wday, value);
    } else
    if (!strcmp(param, "ala4hr")) {
        COPY_BYTE_VALUE(clockCfg.alarms[3].hour, value);
    } else
    if (!strcmp(param, "ala4min")) {
        COPY_BYTE_VALUE(clockCfg.alarms[3].minute, value);
    } else
    if (!strcmp(param, "ala4bep")) {
        COPY_BYTE_VALUE(clockCfg.alarms[3].type, value);
    } else
    if (!strcmp(param, "ala4lop")) {
        COPY_SHORT_VALUE(clockCfg.alarms[3].loops, value);
    } else

    if (!strcmp(param, "ala5day")) {
        COPY_BYTE_VALUE(clockCfg.alarms[4].wday, value);
    } else
    if (!strcmp(param, "ala5hr")) {
        COPY_BYTE_VALUE(clockCfg.alarms[4].hour, value);
    } else
    if (!strcmp(param, "ala5min")) {
        COPY_BYTE_VALUE(clockCfg.alarms[4].minute, value);
    } else
    if (!strcmp(param, "ala5bep")) {
        COPY_BYTE_VALUE(clockCfg.alarms[4].type, value);
    } else
    if (!strcmp(param, "ala5lop")) {
        COPY_SHORT_VALUE(clockCfg.alarms[4].loops, value);
    }

    //setting not found
    else {
        return 2;
    }
    return 0;
}

