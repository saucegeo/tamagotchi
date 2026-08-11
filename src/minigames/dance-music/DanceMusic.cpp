#include "app/Game.h"



void handleDanceMusic() {
    static RTTTL rtttlPlayer;
    static Note currentNote = {0, 0};
    static unsigned long noteStartTime = 0;
    static bool noteActive = false;
    static int currentSongIndex = -1;
    static bool initialized = false;
    static int danceMove = 0;
    static int noteParticles[8] = {0};
    static bool showingSongName = true;
    static const char* songNames[] = {
        "Misato", "Cruel Angel", "Pokemon",
        "Mario", "Zelda", "Tetris",
        "Spirited", "Totoro", "No Scrubs", "Yeah"
    };

    int centerX = canvas.width() / 2 + 30;
    int centerY = canvas.height() / 2 + 10;
    unsigned long elapsed = millis() - stateStartTime;


    if (!initialized) {
        currentSongIndex = random(0, DANCE_SONG_COUNT);
        rtttlPlayer.begin(DANCE_SONGS[currentSongIndex]);
        noteActive = false;
        noteStartTime = millis();
        initialized = true;
        showingSongName = true;
    }


    if (elapsed < 3000 && showingSongName) {
        canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(10, 25);
        canvas.print("NOW PLAYING");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print(songNames[currentSongIndex]);


        for (int i = 0; i < 5; i++) {
            int barHeight = 5 + ((elapsed + i * 100) / 100 % 15);
            canvas.fillRect(15 + i * 20, 60, 10, barHeight, TFT_GREEN);
        }
        return;
    }
    showingSongName = false;


    unsigned long now = millis();
    if (!noteActive || (now - noteStartTime >= (unsigned long)currentNote.duration)) {
        if (rtttlPlayer.getNextNote(currentNote)) {
            if (currentNote.frequency > 0) {
                M5.Speaker.tone(currentNote.frequency, currentNote.duration);


                for (int i = 0; i < 8; i++) {
                    if (noteParticles[i] == 0) {
                        noteParticles[i] = 1;
                        break;
                    }
                }
            } else {
                M5.Speaker.stop();
            }
            noteActive = true;
            noteStartTime = now;
        } else {

            rtttlPlayer.reset();
            noteActive = false;
        }
    }


    int beatPhase = (elapsed / 200) % 4;
    int bounceOffset = 0;
    int rotateOffset = 0;
    int armOffset = 0;

    switch(danceMove) {
        case 0:
            bounceOffset = (beatPhase < 2) ? -8 : 4;
            break;
        case 1:
            rotateOffset = (beatPhase - 2) * 3;
            bounceOffset = abs(beatPhase - 2) * -2;
            break;
        case 2:
            armOffset = (beatPhase < 2) ? -6 : 6;
            bounceOffset = -4;
            break;
    }


    canvas.fillCircle(centerX + rotateOffset, centerY + bounceOffset, 18, TFT_WHITE);


    if (currentNote.frequency > 1500) {
        canvas.setTextColor(TFT_YELLOW, TFT_WHITE);
        canvas.setCursor(centerX - 8 + rotateOffset, centerY - 6 + bounceOffset);
        canvas.print("*");
        canvas.setCursor(centerX + 2 + rotateOffset, centerY - 6 + bounceOffset);
        canvas.print("*");
    } else {
        canvas.fillCircle(centerX - 5 + rotateOffset, centerY - 4 + bounceOffset, 2, TFT_BLACK);
        canvas.fillCircle(centerX + 5 + rotateOffset, centerY - 4 + bounceOffset, 2, TFT_BLACK);
    }


    canvas.drawArc(centerX + rotateOffset, centerY + bounceOffset + 2, 10, 8, 180, 360, TFT_BLACK);


    canvas.drawLine(centerX - 18 + rotateOffset, centerY + bounceOffset - armOffset,
                    centerX - 14 + rotateOffset, centerY + bounceOffset + 8 - armOffset, TFT_WHITE);
    canvas.drawLine(centerX + 18 + rotateOffset, centerY + bounceOffset + armOffset,
                    centerX + 14 + rotateOffset, centerY + bounceOffset + 8 + armOffset, TFT_WHITE);


    for (int i = 0; i < 8; i++) {
        if (noteParticles[i] > 0) {
            int noteY = canvas.height() - (noteParticles[i] * 3);
            int noteX = 20 + (i * 15);
            canvas.setTextColor((i % 2) ? TFT_MAGENTA : TFT_CYAN, TFT_BLACK);
            canvas.setCursor(noteX, noteY);
            canvas.print((i % 3 == 0) ? "o" : "*");
            noteParticles[i]++;
            if (noteParticles[i] > 25) noteParticles[i] = 0;
        }
    }


    canvas.setTextColor(TFT_MAGENTA, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("%s", songNames[currentSongIndex]);


    int timeLeft = (15000 - elapsed) / 1000;
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(canvas.width() - 20, 5);
    canvas.printf("%ds", timeLeft);


    if (M5.BtnA.wasPressed()) {
        danceMove = (danceMove + 1) % 3;
        M5.Speaker.tone(1500, 30);
    }


    if (M5.BtnB.wasPressed()) {
        currentSongIndex = (currentSongIndex + 1) % DANCE_SONG_COUNT;
        rtttlPlayer.begin(DANCE_SONGS[currentSongIndex]);
        M5.Speaker.tone(1200, 30);
        showingSongName = true;
        stateStartTime = millis();
    }

    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(5, canvas.height() - 10);
    canvas.print("A:Dance B:Skip HoldB:Exit");


    if (elapsed > 15000 || M5.BtnB.pressedFor(800)) {
        M5.Speaker.stop();
        initialized = false;
        pet.updateHappiness(3);
        pet.updateEnergy(-2);
        currentState = STATE_IDLE;


        for (int i = 0; i < 8; i++) noteParticles[i] = 0;
    }
}