#ifndef SONGS_H
#define SONGS_H




inline const char* BACKGROUND_MUSIC[] = {

    "TamaTheme:d=4,o=5,b=160:8c6,8e6,8g6,8e6,8c6,8e6,2g6,8a6,8g6,8e6,8c6,2d6,8c6,8e6,8g6,8e6,8c6,8e6,2g6,8f6,8e6,8d6,8c6,2c6",
    "Gentle:d=4,o=5,b=120:8g,8a,8b,8g,8a,8b,8c6,8b,8a,8g,8f,8e,8d,8e,8f,8g,2g,8g,8a,8b,8g,8a,8b,8c6,8b,8a,8g,2g",
    "Peaceful:d=4,o=5,b=100:8c,8d,8e,8f,8g,8a,8g,8f,8e,8d,8c,8d,2e,8f,8g,8a,8b,8a,8g,8f,8e,8f,2g",
    "Meadow:d=8,o=5,b=140:e,g,e,c,d,f,d,4g,e,g,e,c,d,f,d,4c6,c6,b,a,g,f,e,d,4c",
    "Dreamy:d=4,o=5,b=90:c6,e6,g6,2e6,d6,f6,a6,2f6,e6,g6,c7,2g6,f6,a6,c7,2c7"
};
inline const int BACKGROUND_MUSIC_COUNT = 5;


inline const char* MINIGAME_MUSIC[] = {
    "GameStart:d=16,o=6,b=180:c,e,g,c7,8p,c,e,g,c7",
    "Victory:d=8,o=5,b=180:c,e,g,c6,e6,g6,c7,4c7",
    "Playful:d=8,o=5,b=200:c6,c6,g,e6,e6,c,a,4a,g,g,e,c6,c6,a,4g",
    "Energetic:d=8,o=6,b=220:e,d,c,d,4e,e,e,4d,d,d,4e,4g,4g,e,d,c,d,4e,e,e,e,d,d,e,d,2c"
};
inline const int MINIGAME_MUSIC_COUNT = 4;


inline const char* DANCE_SONGS[] = {

    "Misato:d=4,o=7,b=200:e,16e.,d,c,8d.,c.,8c.,d,d,e,16e.,d,c,8d.,c.,8c.,d,d,e,16e.,d,c,8d.,c.,8c.,d,d,e,16e.,d,c,8d.,c.,8c.,16d.",
    "Zankoku:d=8,o=6,b=125:4c,4d#,f.,d#.,f,f,f,a#,g#,16g,f,4g.,4g,4a#,c7.,f.,d#,a#,a#,g,a#,a#.,4c7",
    "Pokemon:d=16,o=5,b=112:32p,f,a#,c6,c#6,c6,c#6,d#6,2f6,a#,c6,8c#6,8f6,8d#6,32c#.6,32d#.6,32c#.6,8c6,8g#.,f,a#,c6,c#6,c6,c#6,d#6,2f6,8a#,c#6,8f6,a,d#6,4g#6",
    "Mario:d=4,o=5,b=100:16e6,16e6,8p,8e6,8p,8c6,8e6,8p,8g6,2p,8g,2p",
    "Zelda:d=4,o=5,b=125:8a#,8f,8f,8a#,8g#,8f#,8g#,8a#.,16a#,16a#,16a#,2g#",
    "Tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a",
    "Spirited:d=8,o=5,b=140:g,a,b,c6,b,a,g,e,g,a,b,c6,b,a,4g,g,a,b,c6,b,a,g,e,c,d,4e",
    "Totoro:d=4,o=5,b=120:8c6,8b,8c6,8d6,8b,8g,8a,8b,2g,8c6,8b,8c6,8d6,8b,8g,2c6",
    "NoScrubs:d=4,o=5,b=112:8e6,8e6,8d6,8c6,8d6,8e6,8d6,8c6,8a,8a,8c6,8d6,8c6,8a,8g,2g,8e6,8e6,8d6,8c6,8d6,8e6,8d6,8c6,8a,2a",
    "Yeah:d=4,o=5,b=140:8e6,8e6,8e6,8c6,8e6,8g6,8e6,8c6,8a,8a,8c6,8e6,2e6,8e6,8e6,8e6,8c6,8e6,8g6,8e6,8c6,2a"
};
inline const int DANCE_SONG_COUNT = 10;


inline const char* SFX_HAPPY = "Happy:d=16,o=6,b=200:c,e,g,c7";
inline const char* SFX_SAD = "Sad:d=8,o=5,b=100:c,b4,a4,g4";
inline const char* SFX_BEEP = "Beep:d=32,o=6,b=300:c,p,c";
inline const char* SFX_SUCCESS = "Success:d=16,o=6,b=180:c,d,e,g,c7";
inline const char* SFX_FAIL = "Fail:d=8,o=5,b=120:g,f,e,d,c";

#endif
