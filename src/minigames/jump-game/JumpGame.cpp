#include "app/Game.h"




static bool isJumping = false;
static float velocityY = 0;
static float gravity = 0.8;
static int jumpProgress;
static int lives;
static int hitAnimTimer;
static bool showInstructions;
static int speedIncrease;
static bool initialized = false;


static int playerY;
static int groundY;
static int jumpHeight = 28;


void updateJumpGamePhysics() {

    playerY += (int)velocityY;
    velocityY += gravity;

    if (playerY >= groundY) {
        playerY = groundY;
        velocityY = 0;
        isJumping = false;
    } else {
        isJumping = true;
    }

    speedIncrease = gameScore / 3;
    objectX -= (2 + speedIncrease);


    if (objectX + 8 >= playerX - 4 && objectX <= playerX + 4) {
        if (playerY >= groundY - 10) {
            lives--;
            hitAnimTimer = 20;
            M5.Speaker.tone(300, 100);


            objectX = canvas.width() + 20;

            if (lives <= 0) {

                currentState = STATE_MINIGAME_RESULT;
                stateStartTime = millis();
                gameActive = false;
                return;
            }
        }
    }


    if (objectX < -10) {
        objectX = canvas.width() + 20;
        gameScore++;
        pet.updateHappiness(1);
        M5.Speaker.tone(1200 + (gameScore * 50), 40);
    }


    if (hitAnimTimer > 0) {
        hitAnimTimer--;
    }
}


void drawJumpGameGraphics() {

    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Score: %d", gameScore);


    for (int i = 0; i < lives; i++) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(canvas.width() - 30 + (i * 10), 5);


         for (int i = 0; i < lives; i++) {

            canvas.fillCircle(canvas.width() - 30 + (i * 10), 8, 3, TFT_RED);
        }
    }


    canvas.drawLine(0, groundY + 5, canvas.width(), groundY + 5, TFT_GREEN);


    canvas.fillCircle(playerX, playerY, 4, TFT_WHITE);
    canvas.fillCircle(playerX - 2, playerY - 1, 1, TFT_BLACK);
    canvas.fillCircle(playerX + 2, playerY - 1, 1, TFT_BLACK);


    if (!isJumping) {
        int legAnim = (millis() / 100) % 4;
        canvas.drawLine(playerX - 2, playerY + 4, playerX - 3, playerY + 7, TFT_WHITE);
        canvas.drawLine(playerX + 2, playerY + 4, playerX + 3, playerY + 7, TFT_WHITE);
    }


    int spikeColor = (hitAnimTimer > 0) ? TFT_ORANGE : TFT_RED;
    canvas.fillRect(objectX, groundY - 6, 8, 8, spikeColor);
    canvas.fillTriangle(objectX, groundY - 6, objectX + 4, groundY - 10, objectX + 8, groundY - 6, spikeColor);


    if (hitAnimTimer > 0 && (hitAnimTimer % 4 < 2)) {
        canvas.fillCircle(playerX, playerY, 5, TFT_RED);
    }


    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A/B:Jump");
}


void handleMinigameJump() {

    if (gameActive == false) {
        initialized = false;
    }

    if (initialized == false && gameActive == true) {
        isJumping = false;
        jumpProgress = 0;
        lives = 3;
        speedIncrease = 0;
        hitAnimTimer = 0;
        showInstructions = true;

        objectX = canvas.width() + 20;
        playerX = 30;
        groundY = canvas.height() - 20;
        playerY = groundY;

        initialized = true;
    }

    unsigned long elapsed = millis() - stateStartTime;


    if (elapsed < 3000 && showInstructions) {
        canvas.setTextColor(TFT_YELLOW, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setCursor(25, 25);
        canvas.print("JUMP!");
        canvas.setTextSize(1);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setCursor(15, 45);
        canvas.print("Avoid the spikes!");
        canvas.setCursor(10, canvas.height() - 15);
        canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
        canvas.print("A or B:Jump");
        return;
    }
    showInstructions = false;


    if ((M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) && !isJumping) {
        velocityY = -12;
        isJumping = true;
        jumpProgress = 0;
        M5.Speaker.tone(1000, 50);
    }


    if (M5.BtnA.wasReleased() || M5.BtnB.wasReleased()) {
        if (velocityY < -3.0) {
            velocityY = -3.0;
        }
    }


    updateJumpGamePhysics();


    if (gameActive == false) return;


    drawJumpGameGraphics();
}