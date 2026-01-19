# 30-DAY SPRINT PLAN: Learn Embedded C++ FOR REAL

**Start:** January 19, 2026  
**Deadline:** February 18, 2026  
**Current Reality:** You've vibecoded everything. You don't understand the code. You're dependent on AI.  
**New Goal:** Actually LEARN embedded C++ while building this gift.

---

## ⚠️ THE HARD TRUTH

You said it yourself: *"I keep relying on AI for everything... I'm not really learning."*

**This ends TODAY.**

### What Vibecoding Looks Like
❌ Ask AI "add a feature"  
❌ Copy-paste code without understanding  
❌ Get error → ask AI to fix  
❌ Repeat until it "works"  
❌ Move to next feature without learning previous one

### What REAL Learning Looks Like
✅ Read documentation FIRST  
✅ Understand the concept BEFORE coding  
✅ Write code yourself (even if wrong)  
✅ Debug your OWN mistakes  
✅ Understand WHY it works, not just THAT it works

---

## 🎯 NEW RULE: AI IS YOUR TUTOR, NOT YOUR CODER

### Allowed AI Usage
✅ "Explain what `M5.Imu.getAccelData()` does"  
✅ "Why does my collision detection fail?"  
✅ "What's the difference between `isPressed()` and `wasPressed()`?"  
✅ "Review my code and explain if it's correct"

### FORBIDDEN AI Usage
❌ "Write a catch game for me"  
❌ "Fix this error" (without showing YOUR attempt first)  
❌ "Add [feature]" (you must try first)  
❌ "Refactor my code" (you must understand it first)

**Rule:** You must ATTEMPT something yourself and FAIL before asking AI for help.

---

## 📅 WEEK-BY-WEEK BREAKDOWN

### WEEK 1 (Jan 19-25): UNDERSTAND WHAT YOU HAVE

**Goal:** Stop vibecoding. Actually understand every line of code in this project.

**NO NEW FEATURES THIS WEEK. Only learning.**

---

#### Day 1 (Sunday): Read & Annotate (4 hours)

**Morning (2 hours):**
1. Open `main.cpp`
2. Read lines 1-300
3. For EVERY line you don't understand, add a comment explaining what you THINK it does
4. Mark lines you're confused about with `// TODO: What does this do?`

**Afternoon (2 hours):**
1. Read [LearnCpp.com Chapter 6](https://www.learncpp.com/cpp-tutorial/local-variables/) (Scope & Variables)
2. Take notes in a notebook (yes, PAPER)
3. Go back to main.cpp and answer your own `// TODO` comments

**Homework:**
- Read lines 300-600 of main.cpp
- Add comments to EVERY function

**Success metric:** You can explain what `currentState = STATE_EATING;` does and WHY it works.

---

#### Day 2 (Monday): Hardware Deep Dive (3 hours)

**Morning (1.5 hours):**
1. Read [M5StickC Plus 2 documentation](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)
2. Answer these questions IN YOUR NOTEBOOK:
   - What is the ESP32?
   - How much RAM does it have?
   - What is an IMU?
   - What does "I2C" mean?
   - Why is there a `setup()` and `loop()` function?

**Afternoon (1.5 hours):**
1. Read [MPU6886 IMU Basics](https://how2electronics.com/interfacing-mpu6886-imu-sensor-esp32/)
2. Answer:
   - What is an accelerometer?
   - What does `accelZ = 1.0` mean?
   - Why does tilting the device change accel values?

**Hands-on Exercise:**
1. Open Serial Monitor
2. Write down sensor values while moving device:
   - Flat on table: accelZ = ?
   - Vertical: accelY = ?
   - Tilted 45°: accelX = ?, accelZ = ?

**Success metric:** You can predict sensor values BEFORE looking at Serial output.

---

#### Day 3 (Tuesday): State Machine Understanding (3 hours)

**Morning (1.5 hours):**
1. Draw the state machine on paper
2. Box for each state (IDLE, EATING, etc.)
3. Arrows showing transitions
4. Label arrows with conditions ("Button A pressed", "2 seconds elapsed")

**Afternoon (1.5 hours):**
1. Read [State Machine Pattern](https://gameprogrammingpatterns.com/state.html)
2. Answer:
   - Why use a state machine instead of if/else everywhere?
   - What is `stateStartTime` used for?
   - How does the game know which state to render?

**Hands-on Exercise:**
1. Add Serial.print to EVERY state transition
2. Play with device and watch state changes
3. Verify your state diagram is correct

**Success metric:** You can explain the state machine without looking at code.

---

#### Day 4 (Wednesday): Sensors & Input (3 hours)

**Morning (1.5 hours):**
1. Study ONLY the sensor reading code (lines ~990-1060 in main.cpp)
2. Answer:
   - How often is IMU data read?
   - What is `M5.update()` doing?
   - Why check `M5.BtnA.wasPressed()` vs `M5.BtnA.isPressed()`?

**Afternoon (1.5 hours):**
1. **YOUR FIRST SOLO CODE:** Modify the flat detection threshold
2. Change `abs(accelZ - 1.0) < 0.2` to `< 0.3`
3. Test on hardware
4. BEFORE asking AI: try to figure out if it made the detection more or less sensitive

**Hands-on Exercise:**
1. Lay device flat
2. Slowly tilt it
3. Note when `Flat:1` changes to `Flat:0` in Serial output
4. Understand the relationship between threshold and sensitivity

**Success metric:** You changed code yourself and understood the result.

---

#### Day 5 (Thursday): Graphics & Display (3 hours)

**Morning (1.5 hours):**
1. Read about [M5 Canvas & Display](https://github.com/m5stack/M5Unified/blob/master/examples/Basic/Display/Display.ino)
2. Answer:
   - What is double buffering?
   - Why use `canvas.pushSprite()` instead of drawing directly?
   - What is RGB565 color format?

**Afternoon (1.5 hours):**
1. Study `drawCharacter()` function
2. **YOUR CODE:** Change the character color from WHITE to CYAN
3. Change: `canvas.fillCircle(..., TFT_WHITE);` to `TFT_CYAN`
4. Compile, upload, test

**Hands-on Exercise:**
1. Draw a red square at position (50, 50)
2. Write this code YOURSELF:
```cpp
canvas.fillRect(50, 50, 20, 20, TFT_RED);
```
3. Test it works

**Success metric:** You drew something on screen WITHOUT copying code.

---

#### Day 6 (Friday): Memory & Persistence (2 hours)

**Morning (1 hour):**
1. Read about [EEPROM on ESP32](https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/)
2. Answer:
   - What is EEPROM?
   - Why does pet state survive power-off?
   - What are "write cycles" and why do they matter?

**Afternoon (1 hour):**
1. Study `Boyfriend.cpp` - the save/load functions
2. Trace through: What happens when you press the power button?

**Success metric:** You understand persistent storage vs RAM.

---

#### Day 7 (Saturday): Week 1 EXAM (4 hours)

**NO AI ALLOWED FOR THIS.**

**Challenge 1: Explain the Code (1 hour)**
Record yourself (or write) explaining:
1. How button presses trigger state changes
2. How the IMU detects "flat"
3. How the game saves pet stats
4. How animations work without blocking

**Challenge 2: Modify Without Breaking (2 hours)**
Make these changes YOURSELF:
1. Change minigame duration from 20s to 30s
2. Make pet hungrier faster (change HUNGER_INTERVAL)
3. Add a new fortune message
4. Test ALL changes work

**Challenge 3: Debug Something (1 hour)**
Intentionally break something, then fix it:
1. Comment out `M5.update();` in loop()
2. What breaks? Why?
3. Fix it and explain

**Success metric:** You passed if you completed ALL challenges without AI help.

---

### WEEK 2 (Jan 26-Feb 1): BUILD YOUR FIRST FEATURE

**Goal:** Add ONE feature completely by yourself.

---

#### Day 8-9 (Mon-Tue): Plan Your Feature (4 hours total)

**Feature to add: "Shake Counter Display"**
- Show step count on idle screen
- Reset every 100 steps

**YOUR TASKS:**
1. Draw mockup on paper (what it looks like)
2. Write pseudocode (not real code, just logic)
3. Identify what variables you need
4. Determine where in code to add it

**NO CODING YET. Only planning.**

---

#### Day 10-11 (Wed-Thu): Implement Your Feature (6 hours total)

**Rules:**
1. Write code YOURSELF first
2. If stuck for 30+ minutes, THEN ask AI specific questions
3. Test after EVERY small change
4. Use Serial.print() to debug

**Steps:**
1. Add `int totalSteps = 0;` variable
2. Increment it when step detected
3. Display it on screen in `handleIdleState()`
4. Test on hardware

**You WILL make mistakes. That's the point.**

---

#### Day 12-13 (Fri-Sat): Custom Character Sprite (5 hours total)

**Learn pixel art:**
1. Watch [Pixel Art Tutorial](https://www.youtube.com/watch?v=gW1G_FLsuEs) (20 min)
2. Download [Piskel](https://www.piskelapp.com/) (free)
3. Create 16x16 sprite of simple character

**Convert to code:**
1. Export as PNG
2. Use [image2cpp](https://javl.github.io/image2cpp/)
3. Replace in sprites.h
4. **UNDERSTAND the byte array format** (read docs)

**Success metric:** You created art AND integrated it yourself.

---

#### Day 14 (Sunday): Week 2 Review (3 hours)

**Git practice:**
1. Commit your changes with clear message
2. Write in README what you added
3. Take screenshot for portfolio

**Documentation:**
1. Add comments explaining your new code
2. Update state diagram if needed

---

### WEEK 3 (Feb 2-8): REFACTOR & POLISH

**Goal:** Clean up code, personalize messages, optimize.

---

#### Day 15-17 (Mon-Wed): Code Refactoring (9 hours total)

**NOW you can refactor, because you UNDERSTAND the code.**

**Task: Extract minigames to separate files**

We've started this - YOU need to:
1. Understand what each minigame does
2. Move code to separate files (following pattern we started)
3. Update main.cpp to include them
4. TEST after each extraction

**Process for EACH minigame:**
1. Read the function thoroughly
2. Add detailed comments
3. Move to new file
4. Compile
5. Test on hardware
6. Fix any issues YOURSELF

---

#### Day 18-19 (Thu-Fri): Personalization (6 hours total)

**Make it YOUR gift:**

1. **Fortune Messages (2 hours):**
   - Write 20 personal messages
   - Inside jokes, memories, compliments
   - Replace fortune array

2. **Custom Music (2 hours):**
   - Pick her favorite song
   - Convert to RTTTL (use online tool)
   - Replace in code
   - Test it sounds right

3. **Colors & Theme (2 hours):**
   - Choose meaningful color palette
   - Replace TFT_WHITE, TFT_CYAN, etc.
   - Make it visually "yours"

---

#### Day 20-21 (Sat-Sun): Bug Hunting & Optimization (6 hours total)

**Test EVERYTHING:**
1. Play every minigame to completion
2. Try every button combination
3. Let it run for 30 minutes
4. Check for memory leaks

**Fix what you find:**
- Document bugs in notebook
- Fix them yourself
- If truly stuck after 1 hour, THEN ask AI

---

### WEEK 4 (Feb 9-15): FINAL POLISH & DOCUMENTATION

---

#### Day 22-23 (Mon-Tue): Final Features (6 hours total)

Choose TWO of:
- WiFi time sync for special days
- Battery-saving sleep mode
- New minigame
- Achievement system

**Build them YOURSELF using what you learned.**

---

#### Day 24-25 (Wed-Thu): Documentation & Portfolio (6 hours total)

**Write README for GitHub:**
1. What you built
2. What you LEARNED
3. Challenges you solved
4. How someone else can build it

**Record demo video:**
- Show all features
- Explain technical decisions
- Demonstrate on hardware

---

#### Day 26-27 (Fri-Sat): Code Review & Final Testing (6 hours total)

**Review every file:**
1. Add missing comments
2. Remove debug code
3. Verify consistent style
4. Check for TODOs

**Final hardware test:**
- Factory reset device
- Upload fresh code
- Verify everything works

---

#### Day 28 (Sunday): PRACTICE EXPLAINING IT (3 hours)

**Internship Interview Prep:**

Practice explaining:
1. "Walk me through your code architecture"
2. "What was the biggest challenge?"
3. "How does the state machine work?"
4. "Explain your memory optimization"
5. "What would you do differently?"

Record yourself. Watch it. Improve.

---

## 📊 LEARNING METRICS

### Week 1: UNDERSTANDING
✅ Can explain every line without AI  
✅ Understand hardware concepts  
✅ Drew state machine diagram  
✅ Modified code successfully  

### Week 2: CREATING  
✅ Added feature from scratch  
✅ Debugged own errors  
✅ Created custom sprite  
✅ Used AI as tutor, not coder  

### Week 3: MASTERING
✅ Refactored code independently  
✅ Personalized thoroughly  
✅ Found and fixed bugs  
✅ Code is clean and commented  

### Week 4: SHOWCASING
✅ Portfolio-ready project  
✅ Can explain to recruiter  
✅ Gift is finished  
✅ You LEARNED embedded C++  

---

## 🚫 THE NO-VIBECODE CONTRACT

**I commit to:**

1. **Try First, Ask Later**
   - Attempt every task myself first
   - Fail for at least 30 minutes before asking AI
   - Ask "why" not "how"

2. **Understand Before Moving On**
   - No copy-paste without comprehension
   - Add comments explaining my understanding
   - Test that I actually know what code does

3. **Build Real Skills**
   - Read documentation before asking
   - Google errors before asking AI
   - Debug with Serial.print() first

4. **Track My Learning**
   - Keep notebook of concepts learned
   - Document struggles and solutions
   - Measure progress weekly

**If I vibecode:** I must delete that code and start over.

---

## 📚 RESOURCES (In Order of Priority)

### Must-Read (Do Week 1)
1. [LearnCpp.com Ch 6-10](https://www.learncpp.com/) - Variables, functions, classes
2. [M5StickC Plus 2 Docs](https://docs.m5stack.com/en/core/M5StickC%20PLUS2) - Your hardware
3. [M5Unified Examples](https://github.com/m5stack/M5Unified/tree/master/examples) - Working code to study

### Should-Read (Do Week 2)
4. [State Machine Pattern](https://gameprogrammingpatterns.com/state.html)
5. [ESP32 Arduino Reference](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
6. [MPU6886 Basics](https://how2electronics.com/interfacing-mpu6886-imu-sensor-esp32/)

### Deep Dive (Do Week 3)
7. "Making Embedded Systems" by Elecia White (Ch 1-4)
8. "Clean Code" by Robert C. Martin (Ch 3, 10)
9. [Embedded Artistry Blog](https://embeddedartistry.com/)

---

## ⚡ DAILY ROUTINE

**Every coding day:**

1. **Morning (30 min):** Read/study before coding
2. **Afternoon (2-3 hours):** Code with breaks every 45 min
3. **Evening (30 min):** Document what you learned

**Every session:**
- Set specific goal ("Today I will understand IMU")
- Work on ONE thing at a time
- Take notes in notebook
- Test frequently

**When stuck:**
1. Read documentation (10 min)
2. Google the error (10 min)
3. Add Serial.print() debug (10 min)
4. Ask AI specific question (not "fix it")

---

## 🎯 FINAL DELIVERABLES (Feb 18)

### The Gift
✅ Working Tamagotchi game  
✅ Personalized sprites, messages, music  
✅ All features tested and polished  
✅ Packaged nicely for girlfriend  

### Your Learning
✅ Understand embedded C++ fundamentals  
✅ Can explain every line of code  
✅ Portfolio-ready project on GitHub  
✅ Demo video showing features  
✅ README with technical depth  

### Interview Readiness
✅ Can whiteboard the state machine  
✅ Explain hardware interfacing  
✅ Discuss memory optimization  
✅ Tell the story of challenges solved  

---

## 💪 MOTIVATION

**You said:** "I'm really mad at myself because I keep relying on AI."

**Good.** That means you're ready to change.

**30 days from now, you'll either:**
- Have a gift your girlfriend loves AND skills that get you an internship
- OR have copy-pasted code you don't understand

**The difference:** Following this plan.

**Every day you vibecode, you rob yourself of learning.**

**Every day you struggle and figure it out, you become a better engineer.**

---

## 🆘 EMERGENCY CONTACT

**If you're truly stuck after trying everything:**

1. **First:** Re-read the relevant docs section
2. **Second:** Ask me a SPECIFIC question with:
   - What you're trying to do
   - What you tried
   - The error/result you got
   - What you think might be wrong

**I will NOT:** Write code for you  
**I WILL:** Explain concepts, point to resources, review YOUR code

---

**START TODAY. No more vibecoding. Time to actually learn.**

Your future self (and your girlfriend) will thank you. 🚀
