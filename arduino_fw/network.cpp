#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <soc/soc.h>

#include "tick-led.h"
#include "storage.h"
#include "network.h"
#include "config.h"

#include "html_lang.h"
#include "html_root.h"

#define RESPONSE_ROOT    1
#define RESPONSE_OK      2
#define RESPONSE_FAILED  3

extern uint8_t state;
extern ClockConfig clockCfg;
extern uint32_t configExit;

WiFiServer server(80);

#define MAX_HTML_SIZE (128*1024)
uint8_t* htmlBuf;
uint32_t htmlLen = 0;
uint32_t htmlPos = 0;

void initNetwork(void) {
    htmlBuf = (uint8_t*) malloc(MAX_HTML_SIZE);
    if (NULL == htmlBuf) {
        Serial.println("html buffer alloction failed");
    } else {
        Serial.printf("html buffer allocated st=%p stw1tc=%p \r \n", &GPIO.status, &GPIO.status_w1tc);
    }

}

void setupWifi(void) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(clockCfg.srvSsid, clockCfg.srvPass);

  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
  Serial.printf("%s / %s\r\n", clockCfg.srvSsid, clockCfg.srvPass);
}

static void handleHttpError(WiFiClient* client) {
    client->println("HTTP/1.1 404 Not Found");
    client->println();
}

static void handleHttpOk(WiFiClient* client, const char* body, uint32_t len, const char binary) {
    char cl[32] = {0};
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    client->println("HTTP/1.1 200 OK");
    if (binary) {
        client->println("Content-type: application/octet-stream");      
    } else {
        client->println("Content-type: text/html");
    }
    if (body != NULL) {
        if (0 == len) {
            len = strlen(body);
        }
        sprintf(cl, "Content-Length: %i", len);
        client->println(cl);
    }

    client->println();
    
    // the content of the HTTP response follows the header:
    if (body != NULL) {
        client->write(body, len);
    }
}

static void replaceString(char* dst, char* dstEnd, char* src) {
    int dstLen = dstEnd - dst;
    int srcLen;
    int dstMax;
    int i;

    if (dst == NULL || dstEnd == NULL || src == NULL) {
        return;
    }
    srcLen = strlen(src);
    if (srcLen < 0) { //zero string is valid - it means an empty string
        return;
    }
    dstMax = strlen(dstEnd);
    
    if (srcLen > dstLen) {
        int diff = srcLen - dstLen;
        int pos = dstMax + diff;

        dstEnd[pos] = 0;
        while (pos > diff) {
            *(dstEnd + pos - 1) = *(dstEnd + pos - 1 - diff);
            pos--;
        }
        dstEnd += diff;
    } else if (dstLen > srcLen) {
        int diff = dstLen - srcLen;
        int pos =  srcLen;
        while (dstMax) {
            dstMax--;
            dst[pos] = dst[pos + diff];
            //printf("copy char at: %d  (%c) %s\n", pos, dst[pos + diff], dst);
            pos++;
        }
        dst[pos] = 0;
        dstEnd -= diff;
    }
    while (dst != dstEnd) {
        *dst = *src;
        dst++;
        src++;
    }            
}

static void replaceStringTag(char* dst, const char* tagName, char* value) {
    char* tag;
    if (tagName == NULL || tagName[0] != '$') {
        return;
    }
    tag = strstr(dst, tagName);
    //tag not found
    if (tag == NULL) {
        return;
    }
    replaceString(tag, tag + strlen(tagName), value);
}

static void replaceStringTagAll(char* dst, const char* tagName, const char* value) {
    char* tag;
    if (tagName == NULL) {
        return;
    }
    while (1) {
        tag = strstr(dst, tagName);
        //tag not found
        if (tag == NULL) {
            return;
        }
        replaceString(tag, tag + strlen(tagName), (char*) value);
        dst = tag + 1;
    }   
}


static void replaceStringTagInt(char* dst, const char* tagName, int32_t val) {
    char value[32] = {0};
    char* tag;
    if (tagName == NULL || tagName[0] != '$') {
        return;
    }
    tag = strstr(dst, tagName);
    //tag not found
    if (tag == NULL) {
        return;
    }
    sprintf(value, "%d", val);
    replaceString(tag, tag + strlen(tagName), value);
}

static void replaceStringTagMulti(char* dst, const char* tagName, uint32_t value, uint32_t valStart, uint32_t valEnd) {
    char nameNew[32];
    char* tag;
    uint32_t i;
    if (tagName == NULL || tagName[0] != '$') {
        return;
    }
    if (value < valStart || value > valEnd) {
        value = valStart;
    }

    for (i = valStart; i <= valEnd; i++) {
        sprintf(nameNew, "%s%d", tagName, i);
        tag = strstr(dst, nameNew);
        //tag found
        if (tag != NULL) {
            if (i == value) {        
                char selected[16] = {0};
                charCopy(selected, "selected", 8);
                replaceString(tag, tag + strlen(nameNew), selected);
            } else {               
                char emptyString[2] = {0};
                replaceString(tag, tag + strlen(nameNew), emptyString);
            }
        }
    }
}

static void translatePage(char* dst) {
    char* tag;
    char** dict = (char**)langroot_en;
    uint16_t dictSize = 0;
    char* start = dst;

    while(dict[dictSize][0] != 0) {
        dictSize++;
    }
    //Serial.printf("dictSize=%d\r\n", dictSize);

    switch (clockCfg.langIndex) {
        case 1: dict = (char**)langroot_cz;
        break;
    }

    while (1) {
        dst = strstr(dst, "$txt_");
        //no more strings to translate found
        if (dst == NULL) {
            return;
        } else 
        // a text to translate was found
        {
            uint16_t index;
            index = (uint16_t) strtol(dst+5, NULL,10);

            //Serial.printf("found tag at byte=%d index=%d %c %c %c %c\r\n", (dst - start), index, *(dst+5), *(dst + 6), *(dst + 7), *(dst + 8) );

            //ensure the dictionary has the required index
            if (index < dictSize) {
                replaceString(dst, dst + 8, dict[index]); // 8 characters to replace: $txt_xxx
            }
            dst++; // skip the replacement and continue searching
        }
    }
}

static void assembleMessagePage(uint16_t msgTitle, uint16_t msgBody) {
    char* dst = (char*) htmlBuf;
    char** dict = (char**)langmisc_en;
    uint16_t dictSize = 0;
    
    while(dict[dictSize][0] != 0) {
        dictSize++;
    }

    switch (clockCfg.langIndex) {
        case 1: dict = (char**)langmisc_cz;
        break;
    }

    charCopy(dst, html_msg, strlen(html_msg));
    if (msgTitle < dictSize) {
        replaceStringTag(dst, "$msgt", dict[msgTitle]);
    }
    if (msgBody < dictSize) {
        replaceStringTag(dst, "$msgb", dict[msgBody]);
    }
}

static void assembleRootPage(void)
{
    char tmp[64];
    char* dst = (char*) htmlBuf;

    charCopy(dst, html_root, strlen(html_root));

    translatePage(dst);

    charCopy(tmp, APP_VERSION, strlen(APP_VERSION));
    replaceStringTag(dst, "$ver", tmp);

    replaceStringTagMulti(dst, "$lang", clockCfg.langIndex, 0, 1);

    replaceStringTag(dst, "$cfs", clockCfg.srvSsid);
    replaceStringTag(dst, "$nts", clockCfg.ntpSsid);
    replaceStringTag(dst, "$nth", clockCfg.ntpHost);
    replaceStringTagMulti(dst, "$csc", clockCfg.colorScheme, 0, 9);
    replaceStringTagMulti(dst, "$ntprat", clockCfg.ntpRate, 0, 3);

    replaceStringTagInt(dst, "$tzof", clockCfg.tzOffset);
    replaceStringTagInt(dst, "$tzds", clockCfg.dsOffset);
    replaceStringTagMulti(dst, "$tzdsr", clockCfg.dsRule, 0, 19);

    replaceStringTagMulti(dst, "$minstl", clockCfg.minuteStyle, 0, 2);
    replaceStringTagMulti(dst, "$secstl", clockCfg.secondStyle, 0, 2);

    replaceStringTagMulti(dst, "$oristl", clockCfg.oRingStyle, 0, 2);
    replaceStringTagMulti(dst, "$vedimg", clockCfg.dimType, 0, 4);
    replaceStringTagMulti(dst, "$vedimfh", clockCfg.dimFrom, 0, 23);
    replaceStringTagMulti(dst, "$vedimfm", clockCfg.dimFromMin /5, 0, 11);
    replaceStringTagMulti(dst, "$vedimth", clockCfg.dimTo, 0, 23);
    replaceStringTagMulti(dst, "$vedimtm", clockCfg.dimToMin / 5, 0, 23);

    replaceStringTagMulti(dst, "$vehana", clockCfg.handAnimStyle, 0, 3);
    replaceStringTagMulti(dst, "$vebrig", clockCfg.brightness, 0, 4);

    replaceStringTagMulti(dst, "$bushp", clockCfg.buttonShort, 0, 2);
    replaceStringTagMulti(dst, "$bumep", clockCfg.buttonMedium, 0, 2);

    //Alarms
    replaceStringTagMulti(dst, "$alapit", clockCfg.alarmPitch, 0, 2);
    
    replaceStringTagMulti(dst, "$ala1day", clockCfg.alarms[0].wday, 0, 10);
    replaceStringTagMulti(dst, "$ala1hr", clockCfg.alarms[0].hour, 0, 23);
    replaceStringTagMulti(dst, "$ala1min", clockCfg.alarms[0].minute / 5, 0, 11);
    replaceStringTagMulti(dst, "$ala1bep", clockCfg.alarms[0].type, 0, 16);
    replaceStringTagMulti(dst, "$ala1lop", getAlarmLoopIndex(clockCfg.alarms[0].loops), 0, 10);

    replaceStringTagMulti(dst, "$ala2day", clockCfg.alarms[1].wday, 0, 10);
    replaceStringTagMulti(dst, "$ala2hr", clockCfg.alarms[1].hour, 0, 23);
    replaceStringTagMulti(dst, "$ala2min", clockCfg.alarms[1].minute / 5, 0, 11);
    replaceStringTagMulti(dst, "$ala2bep", clockCfg.alarms[1].type, 0, 16);
    replaceStringTagMulti(dst, "$ala2lop", getAlarmLoopIndex(clockCfg.alarms[1].loops), 0, 10);

    replaceStringTagMulti(dst, "$ala3day", clockCfg.alarms[2].wday, 0, 10);
    replaceStringTagMulti(dst, "$ala3hr", clockCfg.alarms[2].hour, 0, 23);
    replaceStringTagMulti(dst, "$ala3min", clockCfg.alarms[2].minute / 5, 0, 11);
    replaceStringTagMulti(dst, "$ala3bep", clockCfg.alarms[2].type, 0, 16);
    replaceStringTagMulti(dst, "$ala3lop", getAlarmLoopIndex(clockCfg.alarms[2].loops), 0, 10);

    replaceStringTagMulti(dst, "$ala4day", clockCfg.alarms[3].wday, 0, 10);
    replaceStringTagMulti(dst, "$ala4hr", clockCfg.alarms[3].hour, 0, 23);
    replaceStringTagMulti(dst, "$ala4min", clockCfg.alarms[3].minute / 5, 0, 11);
    replaceStringTagMulti(dst, "$ala4bep", clockCfg.alarms[3].type, 0, 16);
    replaceStringTagMulti(dst, "$ala4lop", getAlarmLoopIndex(clockCfg.alarms[3].loops), 0, 10);

    replaceStringTagMulti(dst, "$ala5day", clockCfg.alarms[4].wday, 0, 10);
    replaceStringTagMulti(dst, "$ala5hr", clockCfg.alarms[4].hour, 0, 23);
    replaceStringTagMulti(dst, "$ala5min", clockCfg.alarms[4].minute / 5, 0, 11);
    replaceStringTagMulti(dst, "$ala5bep", clockCfg.alarms[4].type, 0, 16);
    replaceStringTagMulti(dst, "$ala5lop", getAlarmLoopIndex(clockCfg.alarms[4].loops), 0, 10);

}



static bool applyConfig(const char* line) {
    char* ptr = NULL;
    char tmp[2048];
    bool res;

    if (line == NULL) {
        return false;
    }
    charCopy(tmp, line, strlen(line));
    // decode some html URL characters
    replaceStringTagAll(tmp, "%20", " ");
    replaceStringTagAll(tmp, "%21", "!");
    replaceStringTagAll(tmp, "%22", "\"");
    replaceStringTagAll(tmp, "%23", "#");
    replaceStringTagAll(tmp, "%24", "$");
    replaceStringTagAll(tmp, "%24", "%");
    replaceStringTagAll(tmp, "%26", "&");
    replaceStringTagAll(tmp, "%27", "'");
    replaceStringTagAll(tmp, "%28", "(");
    replaceStringTagAll(tmp, "%29", ")");
    replaceStringTagAll(tmp, "%2A", "*");
    replaceStringTagAll(tmp, "%2B", "+");
    replaceStringTagAll(tmp, "%2C", ",");
    replaceStringTagAll(tmp, "%2D", "-");
    replaceStringTagAll(tmp, "%2E", ".");
    replaceStringTagAll(tmp, "%2F", "/");
    replaceStringTagAll(tmp, "%3A", ":");
    replaceStringTagAll(tmp, "%3B", ";");
    replaceStringTagAll(tmp, "%3C", "<");
    replaceStringTagAll(tmp, "%3D", "=");
    replaceStringTagAll(tmp, "%3E", ">");
    replaceStringTagAll(tmp, "%3F", "?");
    replaceStringTagAll(tmp, "%40", "@");
    
    ptr = strtok(tmp, "&");
    while (ptr) {
        Serial.println(ptr);
        setConfigParam(ptr);
        ptr = strtok(NULL, "&");
    }
    res = storeSaveRecord("config", (uint8_t*) &clockCfg, sizeof(ClockConfig));
    Serial.printf("Config save: %s\r\n", res ? "OK":"Failed");
    return res;
}


void handleWifi(void) {
    bool downloading = false;
    uint8_t statusResponse = 0;;
    uint16_t avCounter = 64;
    uint32_t htmlLenDl = 0;
    bool romDl = false;
    uint32_t recLen = 0;

    delay(10);
    WiFiClient client = server.available();   // listen for incoming clients
    if (client) {                             // if you get a client,
        //Serial.println("New Client.");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        statusResponse = 0;
        while (client.connected()) {            // loop while the client's connected
            int avBytes = client.available();
            if (avBytes > 0) {             // if there's bytes to read from the client,
                 {
                    char c = client.read();             // read a byte, then           
                    if (c == '\n') {                    // if the byte is a newline character
                        // if the current line is blank, you got two newline characters in a row.
                        // that's the end of the client HTTP request, so send a response:
                        if (currentLine.length() == 0) {
                            switch (statusResponse) {
                                // send OK message
                                case RESPONSE_OK: {
                                    assembleMessagePage(0, 1);
                                    handleHttpOk(&client, (const char*)htmlBuf, 0, 0);
                                    state = STATE_CONFIG_EXIT;
                                    configExit = 10;
                                } break;
                                // send Fail message
                                case RESPONSE_FAILED: {
                                    assembleMessagePage(2, 3);
                                    handleHttpOk(&client, (const char*)htmlBuf, 0, 0);
                                } break;
                                //send the main page
                                case RESPONSE_ROOT: {
                                    assembleRootPage();
                                    handleHttpOk(&client, (const char*)htmlBuf, 0, 0);                                      
                                }break;
                                default: {
                                    handleHttpError(&client);
                                }
                            }
                            goto wifi_finish;
                        }
                        // got a new-line character while the currentLine contains some text
                        else 
                        {   
                            Serial.println(currentLine.c_str());  // print the line to the serial monitor

                            if (currentLine.startsWith("GET /apply?") || currentLine.startsWith("GET /lang?")) {
                                Serial.println(">> Detected Config change...");
                                int indexEnd = currentLine.indexOf(" HTTP/1");
                                if (indexEnd > 12) {
                                    bool res;
                                    String params;
                                    int indexStart = currentLine.indexOf("?");
                                    params = currentLine.substring(indexStart + 1, indexEnd);
                                    res = applyConfig(params.c_str());
                                    // parse: GET /apply?cfgssid=tick-led&cfgpwd=&ntpssid=&ntppwd=&ntphost=pool.ntp.org&colsch=2 HTTP/1.1
                                    htmlPos = 0;
                                    htmlLen = 0;
                                    if (res) {
                                        statusResponse = currentLine.startsWith("GET /apply?") ? RESPONSE_OK : RESPONSE_ROOT;
                                    } else {
                                        statusResponse = RESPONSE_FAILED;   
                                    }
                                }
                            } else
                            if (currentLine.startsWith("GET / HTTP/1")) {
                                statusResponse = RESPONSE_ROOT; // send the main / root page
                            } else
                            if (currentLine.startsWith("Content-Length:")) {
                                htmlLen = atoi(currentLine.c_str() + 15);
                                Serial.printf(">> Detected content length=%i\r\n", htmlLen);
                            }
                            
                            currentLine = "";
                        }
                    }
                    else if (c != '\r') {  // if you got anything else but a carriage return character,
                        currentLine += c;      // add it to the end of the currentLine
                    }
                }
            } // client available (avBytes > 0)
            else {
                avCounter --;
                if (avCounter == 0) {
                    Serial.println("No more data available");
                    break;
                } else {
                    delay(1);                   
                }
            }
        } // client connected
wifi_finish:
        // close the connection:
        client.flush();
        client.stop();

        //Serial.println("Client Disconnected.");
    }
}

