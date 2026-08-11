#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

extern bool bgMusicEnabled;
extern bool bgMusicPlaying;
extern int currentBGTrack;

void startBackgroundMusic(int trackIndex);

void updateBackgroundMusic();

void stopBackgroundMusic();

#endif
