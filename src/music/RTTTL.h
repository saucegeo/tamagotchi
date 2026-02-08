#ifndef RTTTL_H
#define RTTTL_H

#include <Arduino.h>

/**
 * RTTTL (Ring Tone Transfer Language) Parser
 * Based on: https://en.wikipedia.org/wiki/Ring_Tone_Transfer_Language
 * 
 * Format: name:defaults:notes
 * Example: "Nintendo:d=4,o=5,b=125:8e6,8e6,8e6,8c6,8e6,8g6,2g"
 */

struct Note {
    float frequency;  // Hz
    int duration;     // milliseconds
};

class RTTTL {
private:
    static const float NOTE_FREQ[16];
    
    String tune;
    int tuneIdx;
    int defaultDuration;
    int defaultOctave;
    int bpm;
    float msecPerWholeNote;
    
    char nextChar();
    void parseDefaults(String defaults);
    
public:
    RTTTL();
    bool begin(const char* rtttlString);
    bool getNextNote(Note& note);
    void reset();
};

#endif // RTTTL_H
