#include "RTTTL.h"

// Note frequency table (A, B, C, D, E, F, G, pad, A#, pad, C#, D#, pad, F#, G#, pad)
const float RTTTL::NOTE_FREQ[16] = {
    440.0,  // A
    493.9,  // B or H
    261.6,  // C
    293.7,  // D
    329.6,  // E
    349.2,  // F
    392.0,  // G
    0.0,    // pad
    
    466.2,  // A#
    0.0,    // pad
    277.2,  // C#
    311.1,  // D#
    0.0,    // pad
    370.0,  // F#
    415.3,  // G#
    0.0     // pad
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
    
    // Split by colons
    int firstColon = fullString.indexOf(':');
    if (firstColon < 0) return false;
    
    int secondColon = fullString.indexOf(':', firstColon + 1);
    if (secondColon < 0) return false;
    
    // Extract defaults and tune
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
            // Save previous value
            if (id == 'o') {
                defaultOctave = val;
            } else if (id == 'd') {
                defaultDuration = val;
            } else if (id == 'b') {
                bpm = val;
            }
            
            // Start new id
            id = c;
            val = 0;
        }
    }
    
    // Save last value
    if (id == 'o') {
        defaultOctave = val;
    } else if (id == 'd') {
        defaultDuration = val;
    } else if (id == 'b') {
        bpm = val;
    }
    
    // Calculate milliseconds per whole note
    // 240000 = 60 sec/min * 4 beats/whole-note * 1000 msec/sec
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
    return '|';  // End marker
}

bool RTTTL::getNextNote(Note& note) {
    // Skip blank characters and commas
    char c = nextChar();
    while (c == ' ') {
        c = nextChar();
    }
    
    // Check for end of tune
    if (c == '|') {
        return false;
    }
    
    // Parse duration
    int duration = 0;
    while (isdigit(c)) {
        duration = duration * 10 + (c - '0');
        c = nextChar();
    }
    if (duration == 0) {
        duration = defaultDuration;
    }
    
    // Parse note name
    char noteName = tolower(c);
    int noteIdx;
    
    if (noteName >= 'a' && noteName <= 'g') {
        noteIdx = noteName - 'a';
    } else if (noteName == 'h') {
        noteIdx = 1;  // H is equivalent to B
    } else if (noteName == 'p') {
        noteIdx = 7;  // Pause
    } else {
        noteIdx = 7;  // Default to pause if invalid
    }
    
    c = nextChar();
    
    // Check for sharp
    if (c == '#') {
        noteIdx += 8;
        c = nextChar();
    }
    
    // Check for duration modifier before octave
    float durationMultiplier = 1.0;
    if (c == '.') {
        durationMultiplier = 1.5;
        c = nextChar();
    }
    
    // Parse octave
    int octave;
    if (c >= '4' && c <= '7') {
        octave = c - '0';
        c = nextChar();
    } else {
        octave = defaultOctave;
    }
    
    // Check for duration modifier after octave
    if (c == '.') {
        durationMultiplier = 1.5;
        c = nextChar();
    }
    
    // Calculate frequency and duration
    note.frequency = NOTE_FREQ[noteIdx] * (1 << (octave - 4));
    note.duration = (int)((msecPerWholeNote / duration) * durationMultiplier);
    
    return true;
}

void RTTTL::reset() {
    tuneIdx = 0;
}
