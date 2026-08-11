#include "RTTTL.h"


const float RTTTL::NOTE_FREQ[16] = {
    440.0,
    493.9,
    261.6,
    293.7,
    329.6,
    349.2,
    392.0,
    0.0,

    466.2,
    0.0,
    277.2,
    311.1,
    0.0,
    370.0,
    415.3,
    0.0
};

RTTTL::RTTTL() {
    tuneIdx = 0;
    defaultDuration = 4;
    defaultOctave = 5;
    bpm = 63;
    msecPerWholeNote = 0;
}

bool RTTTL::begin(const char* rtttlString) {
    if (!rtttlString) return false;

    String fullString = String(rtttlString);


    int firstColon = fullString.indexOf(':');
    if (firstColon < 0) return false;

    int secondColon = fullString.indexOf(':', firstColon + 1);
    if (secondColon < 0) return false;


    String defaults = fullString.substring(firstColon + 1, secondColon);
    tune = fullString.substring(secondColon + 1);

    tuneIdx = 0;
    parseDefaults(defaults);

    return true;
}

void RTTTL::parseDefaults(String defaults) {
    int val = 0;
    char id = ' ';

    for (int i = 0; i < defaults.length(); i++) {
        char c = defaults.charAt(i);
        c = tolower(c);

        if (isdigit(c)) {
            val = val * 10 + (c - '0');
        } else if (isalpha(c)) {

            if (id == 'o') {
                defaultOctave = val;
            } else if (id == 'd') {
                defaultDuration = val;
            } else if (id == 'b') {
                bpm = val;
            }


            id = c;
            val = 0;
        }
    }


    if (id == 'o') {
        defaultOctave = val;
    } else if (id == 'd') {
        defaultDuration = val;
    } else if (id == 'b') {
        bpm = val;
    }



    msecPerWholeNote = 240000.0 / bpm;
}

char RTTTL::nextChar() {
    if (tuneIdx < tune.length()) {
        char c = tune.charAt(tuneIdx);
        tuneIdx++;
        if (c == ',') {
            c = ' ';
        }
        return c;
    }
    return '|';
}

bool RTTTL::getNextNote(Note& note) {

    char c = nextChar();
    while (c == ' ') {
        c = nextChar();
    }


    if (c == '|') {
        return false;
    }


    int duration = 0;
    while (isdigit(c)) {
        duration = duration * 10 + (c - '0');
        c = nextChar();
    }
    if (duration == 0) {
        duration = defaultDuration;
    }


    char noteName = tolower(c);
    int noteIdx;

    if (noteName >= 'a' && noteName <= 'g') {
        noteIdx = noteName - 'a';
    } else if (noteName == 'h') {
        noteIdx = 1;
    } else if (noteName == 'p') {
        noteIdx = 7;
    } else {
        noteIdx = 7;
    }

    c = nextChar();


    if (c == '#') {
        noteIdx += 8;
        c = nextChar();
    }


    float durationMultiplier = 1.0;
    if (c == '.') {
        durationMultiplier = 1.5;
        c = nextChar();
    }


    int octave;
    if (c >= '4' && c <= '7') {
        octave = c - '0';
        c = nextChar();
    } else {
        octave = defaultOctave;
    }


    if (c == '.') {
        durationMultiplier = 1.5;
        c = nextChar();
    }


    note.frequency = NOTE_FREQ[noteIdx] * (1 << (octave - 4));
    note.duration = (int)((msecPerWholeNote / duration) * durationMultiplier);

    return true;
}

void RTTTL::reset() {
    tuneIdx = 0;
}
