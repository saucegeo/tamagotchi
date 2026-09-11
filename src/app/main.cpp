#include <M5Unified.h>
#include <EEPROM.h>
#include "app/Game.h"

void setup() {

    // Enable internal mic in configuratio
    auto cfg = M5.config();
    cfg.internal_mic = true;
    M5.begin(cfg);

    Serial.begin(115200);
    Serial.println("\n=== Tamagotchi Starting ===");

    // Allocate sample memory
    micSamples = (int16_t*)malloc(SAMPLE_COUNT * sizeof(int16_t));

    M5.Mic.begin();

    // M5.begin(cfg) configures and manages internal hardware pins
    // pinMode(4, OUTPUT);
    // digitalWrite(4, HIGH);

    M5.Display.setRotation(1);
    M5.Display.setBrightness(100);

    canvas.setColorDepth(8);
    if (!canvas.createSprite(M5.Display.width(), M5.Display.height())) {

        canvas.setColorDepth(4);
        canvas.createSprite(M5.Display.width(), M5.Display.height());
    }


    EEPROM.begin(512);

    uint8_t magicByte = EEPROM.read(100);
    if (magicByte != 0x42) {

        EEPROM.write(100, 0x42);
        EEPROM.write(0, 0);
        EEPROM.write(1, 8);
        EEPROM.write(2, 4);
        EEPROM.write(3, 4);
        EEPROM.write(4, 12);
        EEPROM.write(5, 0);
        EEPROM.write(6, 0);
        EEPROM.write(7, 10);
        EEPROM.write(8, 0);
        EEPROM.write(9, 0);
        EEPROM.write(10, 0);
        EEPROM.write(11, 0);
        EEPROM.write(12, 1);
        EEPROM.write(13, 1);
        EEPROM.write(14, 0);
        EEPROM.write(15, 0);
        EEPROM.write(16, 0);
        EEPROM.write(17, 0);
        EEPROM.commit();
    }


    pet.loadFromEEPROM();

    clockSetTime = millis();
    hatchTime = millis();

    M5.Imu.begin();


    Serial.println("=== Setup Complete ===");


    M5.Speaker.tone(1047, 100);
    delay(120);
    M5.Speaker.tone(1319, 100);
    delay(120);
    M5.Speaker.tone(1568, 100);
    delay(120);
    M5.Speaker.tone(2093, 150);
    delay(200);


    startBackgroundMusic(0);


    delay(100);
}


void loop() {
    unsigned long now = millis();

    yield();

    M5.update();

    static unsigned long lastDebugPrint = 0;
    if (now - lastDebugPrint > 2000) {
        Serial.printf("State:%d Flat:%d TiltUp:%d AccZ:%.2f AccY:%.2f BtnA:%d BtnB:%d\n",
                      currentState, isFlat, isTiltedDown, isTiltedUp, accelZ, accelY,
                      M5.BtnA.isPressed(), M5.BtnB.isPressed());
        lastDebugPrint = now;
    }


    M5.Imu.getAccel(&accelX, &accelY, &accelZ);
    M5.Imu.getGyro(&gyroX, &gyroY, &gyroZ);


    if (M5.Mic.isEnabled() && micSamples != nullptr) {
        // Record at 16000 Hz into micSamples and only update micLevel when new data
        if (M5.Mic.isEnabled() && micSamples != nullptr) {
        // Record at 16000 Hz into micSamples. Only update micLevel when new data arrives
        if (M5.Mic.record(micSamples, SAMPLE_COUNT, 16000)) {
            int32_t sum = 0;
            for (int i = 0; i < SAMPLE_COUNT; i++) {
                sum += abs(micSamples[i]);
            }
            micLevel = sum / SAMPLE_COUNT;
        } 
    }
}


    int batteryLevel = M5.Power.getBatteryLevel();
    bool lowBattery = batteryLevel < 20;




    isFlat = (abs(accelZ - 1.0) < 0.2 && abs(accelX) < 0.2 && abs(accelY) < 0.2);






    if (isFlat && currentState == STATE_IDLE &&
        (now - lastAutoStateChange > AUTO_STATE_COOLDOWN)) {
        currentState = STATE_SLEEPING;
        stateStartTime = now;
        lastAutoStateChange = now;
    }


    if (!isFlat && currentState == STATE_SLEEPING) {
        currentState = STATE_IDLE;
        lastAutoStateChange = now;
    }


    isTiltedDown = (accelY > 0.7);
    isTiltedUp = (accelY < -0.7);   

    if (isTiltedDown && currentState == STATE_IDLE &&
        (now - lastAutoStateChange > AUTO_STATE_COOLDOWN)) {
        currentState = STATE_STARGAZING;
        stateStartTime = now;
        lastAutoStateChange = now;
    }


    float totalAccel = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
    if (totalAccel > 2.0) {
        if (now - lastShakeTime > 300) {
            shakeCount++;
            lastShakeTime = now;
            if (currentState == STATE_SHAKE_CLEAN) {
                M5.Speaker.tone(1500, 30);
            }
        }
    }



    static float lastAccelZ = 0;
    if (accelZ > 1.2 && lastAccelZ < 1.0) {
        if (now - lastStepTime > 400 && now - lastStepTime < 1200) {
            stepCount++;
            if (stepCount % 10 == 0) {
                pet.updateEnergy(1);
            }
        }
        lastStepTime = now;
    }
    lastAccelZ = accelZ;







    int animSpeed = lowBattery ? 400 : 200;



    if (now - lastHungerUpdate > HUNGER_INTERVAL) {
        pet.hunger--;
        if (pet.hunger < 0) pet.hunger = 0;
        lastHungerUpdate = now;
    }

    if (now - lastHappinessUpdate > HAPPINESS_INTERVAL) {
        pet.happiness--;
        if (pet.happiness < 0) pet.happiness = 0;
        lastHappinessUpdate = now;
    }

    if (now - lastEnergyUpdate > ENERGY_INTERVAL) {
        pet.energy--;
        if (pet.energy < 0) pet.energy = 0;
        lastEnergyUpdate = now;
    }


    if (currentState != STATE_DEAD) {
        schedulePoop();
    }


    if (pet.stage != STAGE_ADULT && currentState != STATE_DEAD) {
        checkEvolution();
    }



    if (currentState == STATE_IDLE && pet.stage >= STAGE_CHILD) {
        if (nextDisciplineCall == 0) {
            nextDisciplineCall = now + random(1800000, 3600000);
        }
        if (now >= nextDisciplineCall) {

            currentState = STATE_DISCIPLINE_CALL;
            stateStartTime = now;
            nextDisciplineCall = 0;
        }
    }



    updateSicknessSystem();



    if (currentState != STATE_DEAD && !isDead) {
        if (checkDeathConditions()) {
            isDead = true;
            currentState = STATE_DEAD;
            stateStartTime = now;

            M5.Speaker.tone(800, 200);
            delay(200);
            M5.Speaker.tone(600, 200);
            delay(200);
            M5.Speaker.tone(400, 400);
        }
    }


    if (now - lastAnimUpdate > animSpeed) {
        animationFrame++;
        lastAnimUpdate = now;
    }



    static GameState lastMusicState = STATE_IDLE;
    if (currentState != lastMusicState) {

        if (currentState == STATE_IDLE || currentState == STATE_FEED_MENU ||
            currentState == STATE_MINIGAME_MENU || currentState == STATE_SLEEPING) {
            if (!bgMusicPlaying) {
                startBackgroundMusic(currentBGTrack);
            }
        } else if (currentState == STATE_MINIGAME_CATCH || currentState == STATE_MINIGAME_JUMP ||
                   currentState == STATE_DANCE_MUSIC) {

            stopBackgroundMusic();
        }
        lastMusicState = currentState;
    }


    if (currentState == STATE_IDLE || currentState == STATE_FEED_MENU ||
        currentState == STATE_MINIGAME_MENU || currentState == STATE_SLEEPING) {
        updateBackgroundMusic();
    }


    canvas. fillScreen(TFT_BLACK);


    switch (currentState) {
        case STATE_IDLE:
            handleIdleState();
            break;
        case STATE_EATING:
            handleEatingState();
            break;
        case STATE_PLAYING:
            handlePlayingState();
            break;
        case STATE_SLEEPING:
            handleSleepingState();
            break;
        case STATE_ATTENTION_NEEDED:
            handleAttentionNeededState();
            break;
        case STATE_LOVE_NOTE:
            handleLoveNoteState();
            break;
        case STATE_MINIGAME_MENU:
            handleMinigameMenu();
            break;
        case STATE_MINIGAME_CATCH:
            handleMinigameCatch();
            break;
        case STATE_MINIGAME_JUMP:
            handleMinigameJump();
            break;
        case STATE_MINIGAME_RESULT:
            handleMinigameResult();
            break;
        case STATE_DANCE_MUSIC:
            handleDanceMusic();
            break;
        case STATE_STARGAZING:
            handleStargazing();
            break;

        case STATE_BLOW_CANDLE:
            handleBlowCandle();
            break;

        case STATE_FORTUNE_COOKIE:
            handleFortuneCookie();
            break;

        case STATE_SHAKE_CLEAN:
            handleShakeClean();
            break;
        case STATE_LOVE_METER:
            handleLoveMeter();
            break;
        case STATE_DEAD:
            handleDeathState();
            break;
        case STATE_DISCIPLINE_CALL:
            handleDisciplineCall();
            break;
        case STATE_FEED_MENU:
            handleFeedMenu();
            break;
        case STATE_SICK:
            drawCharacter(canvas.width() / 2 + 30, canvas.height() / 2 + 10);
            break;
    }


    if (currentState != STATE_MINIGAME_MENU &&
        currentState != STATE_MINIGAME_CATCH &&
        currentState != STATE_MINIGAME_JUMP &&
        currentState != STATE_MINIGAME_RESULT &&
        currentState != STATE_FORTUNE_COOKIE &&
        currentState != STATE_DANCE_MUSIC &&
        currentState != STATE_STARGAZING &&
        currentState != STATE_SHAKE_CLEAN &&
        currentState != STATE_BLOW_CANDLE &&
        currentState != STATE_LOVE_METER &&
        currentState != STATE_DEAD &&
        currentState != STATE_DISCIPLINE_CALL &&
        currentState != STATE_FEED_MENU) {
        drawStatsBar();


        if (lowBattery) {
            canvas.setTextColor(TFT_RED, TFT_BLACK);
            canvas.setCursor(5, 72);
            canvas.print("LOW BATTERY!");
        }
    }


    canvas.pushSprite(0, 0);


    static unsigned long lastSave = 0;
    if (now - lastSave > 60000) {
        EEPROM.write(0, pet.sleeping);
        EEPROM.write(1, pet.sleep);
        EEPROM.write(2, pet.happiness);
        EEPROM.write(3, pet.hunger);
        EEPROM.write(4, pet.energy);
        EEPROM.write(5, pet.age);
        EEPROM.write(6, pet.mood);
        EEPROM.write(7, pet.weight);
        EEPROM.write(8, pet.discipline);
        EEPROM.write(9, pet.careMistakes);
        EEPROM.write(10, pet.poopCount);
        EEPROM.write(11, pet.isSick);
        EEPROM.write(12, (int)pet.stage);
        EEPROM.write(13, pet.lightsOn);
        EEPROM.write(14, pet.hasToothache);
        EEPROM.write(15, pet.isSulking);
        EEPROM.write(16, pet.snackStreak);
        EEPROM.write(17, pet.medicineNeeded);
        EEPROM.commit();
        lastSave = now;
    }


    delay(50);
}
