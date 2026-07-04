#include "../GameState.h"

/*
 * MINIGAME: Jump Over Obstacles
 * 
 * Objective: Time your jumps to avoid obstacles
 * Controls: Button A or B = Jump
 * Game Over: If you hit an obstacle 3 times
 * Scoring: +1 point per obstacle passed
 */

// Minigame state variables
static bool isJumping = false; // Track if the player is currently jumping
static float velocityY = 0; // Velocity is initially 0, and will change based on gravity
static float gravity = 0.8; 
static int jumpProgress;
static int lives; // Track remaining lives (3 max)
static int hitAnimTimer;
static bool showInstructions;
static int speedIncrease;
static bool initialized = false; // Game is not initialized yet

// We share the Y-coordinates so both physics and graphics can use the same value
static int playerY; // Player's vertical position
static int groundY; // Ground level
static int jumpHeight = 28; // Maximum jump height

// Function for game physics and jump mechanics
void updateJumpGamePhysics() {
    
    playerY += (int)velocityY; // Update player position based on velocity
    velocityY += gravity; // Apply gravity to the velocity (pulls the player down)

    if (playerY >= groundY) {
        playerY = groundY; // Reset to ground level if below
        velocityY = 0; // Stop downward movement
        isJumping = false; // Player is no longer jumping and landed
    } else {
        isJumping = true; // Player is in the air
    }

    speedIncrease = gameScore / 3;  // Increase speed every 3 points
    objectX -= (2 + speedIncrease); // Move obstacle leftward towards the player

    // Collision detection
    if (objectX + 8 >= playerX - 4 && objectX <= playerX + 4) { // Check if the player is in obstacle's horizontal range
        if (playerY >= groundY - 10) { // Check if the player is at or below the spike tip
            lives--; // Player loses a life
            hitAnimTimer = 20; // Start hit animation timer
            M5.Speaker.tone(300, 100); // Play hit sound
            
            // Reset obstacle position
            objectX = canvas.width() + 20;
            
            if (lives <= 0) {
                // Game over
                currentState = STATE_MINIGAME_RESULT;
                stateStartTime = millis();
                gameActive = false;
                return;
            }
        }
    }

    // Obstacle passed successfully
    if (objectX < -10) {
        objectX = canvas.width() + 20; // Reset obstacle position
        gameScore++; // Increment score
        boyfriend.updateHappiness(1); // Increase boyfriend's happiness
        M5.Speaker.tone(1200 + (gameScore * 50), 40); // Play success sound
    }

    // Decrement hit timer
    if (hitAnimTimer > 0) {
        hitAnimTimer--;
    }
}

// Function to draw the game graphics
void drawJumpGameGraphics() {
// Display score
    canvas.setTextColor(TFT_CYAN, TFT_BLACK);
    canvas.setCursor(5, 5);
    canvas.printf("Score: %d", gameScore);
    
    // Draw lives as hearts
    for (int i = 0; i < lives; i++) {
        canvas.setTextColor(TFT_RED, TFT_BLACK);
        canvas.setCursor(canvas.width() - 30 + (i * 10), 5);
        
        // Draw lives as red dots (Temporary fix)
         for (int i = 0; i < lives; i++) {
            // Draw a red circle for each life at the top right
            canvas.fillCircle(canvas.width() - 30 + (i * 10), 8, 3, TFT_RED);
        }
    }
    
    // Draw ground line
    canvas.drawLine(0, groundY + 5, canvas.width(), groundY + 5, TFT_GREEN);
    
    // Draw player character
    canvas.fillCircle(playerX, playerY, 4, TFT_WHITE);
    canvas.fillCircle(playerX - 2, playerY - 1, 1, TFT_BLACK);  // Eye
    canvas.fillCircle(playerX + 2, playerY - 1, 1, TFT_BLACK);
    
    // Running animation (legs)
    if (!isJumping) {
        int legAnim = (millis() / 100) % 4;
        canvas.drawLine(playerX - 2, playerY + 4, playerX - 3, playerY + 7, TFT_WHITE);
        canvas.drawLine(playerX + 2, playerY + 4, playerX + 3, playerY + 7, TFT_WHITE);
    }
    
    // Draw spiky obstacle
    int spikeColor = (hitAnimTimer > 0) ? TFT_ORANGE : TFT_RED;
    canvas.fillRect(objectX, groundY - 6, 8, 8, spikeColor);
    canvas.fillTriangle(objectX, groundY - 6, objectX + 4, groundY - 10, objectX + 8, groundY - 6, spikeColor);
    
    // Hit animation (flash)
    if (hitAnimTimer > 0 && (hitAnimTimer % 4 < 2)) {
        canvas.fillCircle(playerX, playerY, 5, TFT_RED);
    }
    
    // Show controls text
    canvas.setTextColor(TFT_DARKGREY, TFT_BLACK);
    canvas.setCursor(10, canvas.height() - 10);
    canvas.print("A/B:Jump");
}

// Main function to handle the Jump minigame state
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
    
    // Instructions screen for the first 3 seconds
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
    
    // Check for jumping (Full Jump)
    if ((M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) && !isJumping) {
        velocityY = -12; // Big jump
        isJumping = true;
        jumpProgress = 0;
        M5.Speaker.tone(1000, 50);
    }

    // Check for button release (Short Jump)
    if (M5.BtnA.wasReleased() || M5.BtnB.wasReleased()) {
        if (velocityY < -3.0) {
            velocityY = -3.0; // Limit upward velocity for short jump
        }
    }

    // Run the math to update the physics and check for collisions
    updateJumpGamePhysics();

    // Prevent drawing if the physics engine just ended the game
    if (gameActive == false) return;

    // Paint the screen (Player, obstacles, score, lives, etc.)
    drawJumpGameGraphics();
}