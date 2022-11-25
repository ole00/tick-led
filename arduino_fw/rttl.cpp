//repurposed from 
//https://github.com/ponty/arduino-rtttl-player/tree/master/rtttl


#include <stdint.h>
#include <Arduino.h>

#include "tick-led.h"
#include "rttl.h"

const uint16_t mus_notes[] =
{    0, //
    NOTE_C4, //
    NOTE_CS4, //
    NOTE_D4, //
    NOTE_DS4, //
    NOTE_E4, //
    NOTE_F4, //
    NOTE_FS4, //
    NOTE_G4, //
    NOTE_GS4, //
    NOTE_A4, //
    NOTE_AS4, //
    NOTE_B4, //

    NOTE_C5, //
    NOTE_CS5, //
    NOTE_D5, //
    NOTE_DS5, //
    NOTE_E5, //
    NOTE_F5, //
    NOTE_FS5, //
    NOTE_G5, //
    NOTE_GS5, //
    NOTE_A5, //
    NOTE_AS5, //
    NOTE_B5, //

    NOTE_C6, //
    NOTE_CS6, //
    NOTE_D6, //
    NOTE_DS6, //
    NOTE_E6, //
    NOTE_F6, //
    NOTE_FS6, //
    NOTE_G6, //
    NOTE_GS6, //
    NOTE_A6, //
    NOTE_AS6, //
    NOTE_B6, //

    NOTE_C7, //
    NOTE_CS7, //
    NOTE_D7, //
    NOTE_DS7, //
    NOTE_E7, //
    NOTE_F7, //
    NOTE_FS7, //
    NOTE_G7, //
    NOTE_GS7, //
    NOTE_A7, //
    NOTE_AS7, //
    NOTE_B7, //

    2*NOTE_C7, //
    2*NOTE_CS7, //
    2*NOTE_D7, //
    2*NOTE_DS7, //
    2*NOTE_E7, //
    2*NOTE_F7, //
    2*NOTE_FS7, //
    2*NOTE_G7, //
    2*NOTE_GS7, //
    2*NOTE_A7, //
    2*NOTE_AS7, //
    2*NOTE_B7, //
    0
};


void playRttl(RttlContext* rttl, bool doDelay) {
    // Absolutely no error checking in here
    uint8_t octave_offset;
    char *p;
    byte default_dur = 4;
    byte default_oct = 6;
    int bpm = 63;
    int num;
    long wholenote;
    long duration;
    byte note;
    byte scale;

    if (rttl == NULL) {
        return;   
    }

    if (rttl->state == RTTL_IDLE) {
        if (doDelay) {
            delay(rttl->timeSlice);
        }
        return;
    } else
    if (rttl->state == RTTL_STOP || rttl->loops == 0) {
        rttl->state = RTTL_IDLE;
        rttl->duration = 0;
        rttl->songPos = 0;
        rttl->loops = 0;
        //buzzer off
        setBeeperVolume(0);
        if (doDelay) {
            delay(rttl->timeSlice);
        }
        return;
    }

    if (rttl-> duration > rttl->timeSlice) {
        if (doDelay) {
            delay(rttl->timeSlice);
        }
        rttl->duration -= rttl->timeSlice;
        return;
    }

    octave_offset = rttl->octave;
    p = rttl->song + rttl->songPos;


    //time to play a new note
    //stop previous note
    setBeeperVolume(0);

    //starting a song -> read the song's defaults
    if (rttl->songPos == 0) {
        // format: d=N,o=N,b=NNN:
        // find the start (skip name, etc)

        while (*p  != ':')
            p++; // ignore name
        p++; // skip ':'
    
        // get default duration
        if (*p == 'd') {
            p++;
            p++; // skip "d="
            num = 0;
            while (isdigit(*p)) {
                num = (num * 10) + (*p - '0');
                p++;
            }
            if (num > 0)
                default_dur = num;
            p++; // skip comma
        }
    
        // get default octave
        if (*p == 'o') {
            p++;
            p++; // skip "o="
            num = *p - '0';
            p++;
            if (num >= 3 && num <= 7)
                default_oct = num;
            p++; // skip comma
        }
    
        // get BPM
        if (*p == 'b') {
            p++;
            p++; // skip "b="
            num = 0;
            while (isdigit(*p)) {
                num = (num * 10) + (*p - '0');
                p++;
            }
            bpm = num;
            p++; // skip colon
        }
        rttl->bpm = bpm;
        rttl->defaultOct = default_oct;
        rttl->defaultDur = default_dur;
        
    } else {
        bpm = rttl->bpm;
        default_dur = rttl->defaultDur;
        default_oct = rttl->defaultOct;
    }

    // BPM usually expresses the number of quarter notes per minute
    wholenote = (60 * 1000L / bpm) * 4; // this is the time for whole note (in milliseconds)


    // now begin a note -> if there is one
    if (*p){
        // first, get note duration, if available
        num = 0;
        while (isdigit(*p)) {
            num = (num * 10) + (*p - '0');
            p++;
        }

        if (num)
            duration = wholenote / num;
        else
            duration = wholenote / default_dur; // we will need to check if we are a dotted note after

        // now get the note
        note = 0;

        switch (*p) {
        case 'c':
            note = 1;
            break;
        case 'd':
            note = 3;
            break;
        case 'e':
            note = 5;
            break;
        case 'f':
            note = 6;
            break;
        case 'g':
            note = 8;
            break;
        case 'a':
            note = 10;
            break;
        case 'b':
            note = 12;
            break;
        case 'p':
        default:
            note = 0;
        }
        p++;

        // now, get optional '#' sharp
        if (*p == '#') {
            note++;
            p++;
        }

        // now, get optional '.' dotted note
        if (*p == '.') {
            duration += duration / 2;
            p++;
        }

        // now, get scale
        if (isdigit(*p)) {
            scale = *p - '0';
            p++;
        }
        else {
            scale = default_oct;
        }

        scale += octave_offset;

        if (*p == ',')
            p++; // skip comma for next note (or we may be at the end)

        // now play the note
        //Serial.printf("dur: %d\r\n", duration);
        if (note) {
            uint16_t freq = mus_notes[(scale - 4) * 12 + note];
            //before wa start a new note let's have a small pause to distinguish the stop of the previous note
            delay(1);
            //set note frequency
            setBeeperFreq(freq);
            //buzzer on
            setBeeperVolume(100);

            rttl->duration = duration - rttl->timeSlice;
            if (rttl->timeSlice > 1) {
                if (doDelay) {
                    delay(rttl->timeSlice - 1);
                }
                rttl->duration--;
            }
        }
        else
        {
            rttl->duration = duration - rttl->timeSlice;
            if (doDelay) {
                delay(rttl->timeSlice);
            }
        }
    } else
    //no more notes to play
    {
        //Serial.println("done");
        rttl->loops--;
        rttl->duration = 0;
        p = rttl->song;
        if (!rttl->loops) {
            rttl->state = RTTL_IDLE;
        }       
    }
    rttl->songPos = p - rttl->song;
}

void stopRttl(RttlContext* rttl) {
    if (rttl == NULL) {
        return;   
    }
    rttl->state = RTTL_STOP;
}

