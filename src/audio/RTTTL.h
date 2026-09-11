#ifndef RTTTL_H
#define RTTTL_H

#include <Arduino.h>


struct Note {
    float frequency;
    int duration;
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

#endif
