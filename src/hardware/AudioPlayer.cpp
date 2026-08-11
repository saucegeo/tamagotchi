#include "AudioPlayer.h"
#include <M5Unified.h>
#include "audio/RTTTL.h"
#include "content/Songs.h"

RTTTL bgMusicPlayer;
bool bgMusicEnabled = false;
bool bgMusicPlaying = false;
int currentBGTrack = 0;
unsigned long lastNoteTime = 0;
Note currentBGNote;


void startBackgroundMusic(int trackIndex) {
    if (!bgMusicEnabled) return;

    currentBGTrack = trackIndex % BACKGROUND_MUSIC_COUNT;
    bgMusicPlayer.begin(BACKGROUND_MUSIC[currentBGTrack]);
    bgMusicPlaying = true;
    lastNoteTime = millis();
}

void updateBackgroundMusic() {
    if (!bgMusicEnabled || !bgMusicPlaying) return;

    unsigned long now = millis();


    if (now - lastNoteTime >= currentBGNote.duration) {

        if (bgMusicPlayer.getNextNote(currentBGNote)) {

            if (currentBGNote.frequency > 0) {
                M5.Speaker.tone(currentBGNote.frequency, currentBGNote.duration);
            }
            lastNoteTime = now;
        } else {

            currentBGTrack = (currentBGTrack + 1) % BACKGROUND_MUSIC_COUNT;
            startBackgroundMusic(currentBGTrack);
        }
    }
}

void stopBackgroundMusic() {
    bgMusicPlaying = false;
    M5.Speaker.stop();
}
