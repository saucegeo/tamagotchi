# M5StickC Plus 2 Hardware Guide

**For someone with VHDL background:** Think of this as a soft-core processor with peripheral IP blocks.

---

## Hardware Overview

### M5StickC Plus 2 Specifications

```
┌─────────────────────────────────────────────┐
│  🖥️  Display (1.14" LCD)                    │
│      135x240 pixels, ST7789 driver          │
│      SPI interface, 16-bit color            │
├─────────────────────────────────────────────┤
│  🧠  ESP32-PICO-V3-02                       │
│      Dual-core Xtensa LX6 @ 240MHz          │
│      320KB SRAM, 4MB Flash                  │
│      WiFi + Bluetooth                       │
├─────────────────────────────────────────────┤
│  📐  IMU (MPU6886)                          │
│      6-axis: 3-axis accel + 3-axis gyro     │
│      I2C address 0x68                       │
│      Range: ±2G accel, ±2000°/s gyro        │
├─────────────────────────────────────────────┤
│  🔋  Battery (200mAh LiPo)                  │
│      AXP2101 power management               │
│      USB-C charging                         │
│      ~2 hours runtime                       │
├─────────────────────────────────────────────┤
│  🎤  Microphone (SPM1423)                   │
│      PDM digital mic                        │
│      I2S interface                          │
├─────────────────────────────────────────────┤
│  🔊  Buzzer (Passive)                       │
│      PWM-driven speaker                     │
│      GPIO2                                  │
├─────────────────────────────────────────────┤
│  🔘  Buttons                                 │
│      Button A (GPIO37)                      │
│      Button B (GPIO39)                      │
│      Power Button (handled by AXP2101)      │
├─────────────────────────────────────────────┤
│  🔴  LED (Red)                              │
│      GPIO19                                 │
└─────────────────────────────────────────────┘
```

---

## ESP32 Architecture (For VHDL Background)

### Block Diagram
```
┌──────────────────────────────────────────────────────┐
│                    ESP32 SoC                         │
│  ┌────────────┐           ┌────────────┐            │
│  │ CPU Core 0 │           │ CPU Core 1 │            │
│  │  (240 MHz) │◄─────────►│  (240 MHz) │            │
│  └──────┬─────┘           └─────┬──────┘            │
│         │                       │                    │
│         ├───────────────────────┤                    │
│         │                       │                    │
│  ┌──────▼───────────────────────▼──────┐            │
│  │         Shared L2 Cache             │            │
│  │         (& Memory Controller)        │            │
│  └──────┬───────────────────────┬──────┘            │
│         │                       │                    │
│  ┌──────▼──────┐         ┌─────▼──────┐            │
│  │  SRAM       │         │   Flash    │            │
│  │  (320KB)    │         │   (4MB)    │            │
│  └─────────────┘         └────────────┘            │
│                                                      │
│  ┌──────────────────────────────────────────────┐  │
│  │           Peripheral Bus (APB)               │  │
│  └┬────┬────┬────┬────┬────┬────┬────┬────┬────┘  │
│   │    │    │    │    │    │    │    │    │       │
│  I2C  SPI  I2S  PWM  ADC GPIO UART Timer  RTC     │
└──────────────────────────────────────────────────────┘
```

**Key Differences from VHDL/FPGA:**
- **Software-configured peripherals** (not hardwired logic)
- **Interrupt-driven I/O** (not continuous polling)
- **Memory-mapped registers** (write to addresses to control hardware)

---

## Communication Protocols

### 1. I2C (Inter-Integrated Circuit)

**Used by:** IMU (MPU6886)

**Physical Interface:**
- SDA (Serial Data) - bidirectional data line
- SCL (Serial Clock) - clock from master (ESP32)
- Both lines pulled high with resistors

**How It Works:**
```
Master (ESP32)          Slave (IMU)
      │                      │
START ├──────────────────────┤
      │ Address (0x68)       │
      ├──────────────────────┤ ACK
      │ Register (0x3B)      │
      ├──────────────────────┤ ACK
      │ Read Data            │
      ├──────────────────────┤ Data Byte
STOP  ├──────────────────────┤
```

**Code Example:**
```cpp
// Read accelerometer X-axis (register 0x3B)
Wire.beginTransmission(0x68);  // IMU address
Wire.write(0x3B);              // Register to read
Wire.endTransmission(false);   // Repeated start
Wire.requestFrom(0x68, 2);     // Read 2 bytes
int16_t raw = Wire.read() << 8 | Wire.read();
float accelX = raw / 16384.0;  // Convert to G-forces
```

**In VHDL terms:** Think of I2C as a serial bus with address decoding and ACK/NACK handshaking.

---

### 2. SPI (Serial Peripheral Interface)

**Used by:** Display (ST7789)

**Physical Interface:**
- MOSI (Master Out, Slave In) - data from ESP32
- MISO (Master In, Slave Out) - data to ESP32
- SCK (Serial Clock) - clock from master
- CS (Chip Select) - active low, selects device

**Why SPI for display:**
- **Fast:** Up to 40MHz clock (I2C is ~400kHz)
- **Full-duplex:** Can send commands and data simultaneously
- **No addressing:** CS pin selects device

**Waveform:**
```
CS   ────┐                          ┌────
         └──────────────────────────┘
SCK  ──┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌──
       └─┘└─┘└─┘└─┘└─┘└─┘└─┘└─┘└─┘
MOSI ───< D7 >< D6 >< D5 >< D4 >< D3 >──
```

**In VHDL terms:** SPI is like a shift register with external clock.

---

### 3. I2S (Inter-IC Sound)

**Used by:** Microphone (PDM)

**What is PDM (Pulse Density Modulation):**
- Stream of 1s and 0s
- Density of 1s represents amplitude
- Sampled at high frequency (1-3MHz)
- Filtered to get audio

**Why not analog:**
- Digital = noise immune
- No ADC needed
- Better SNR (signal-to-noise ratio)

---

### 4. PWM (Pulse Width Modulation)

**Used by:** Buzzer/Speaker

**Principle:**
```
50% duty cycle (medium volume)
    ┌──┐  ┌──┐  ┌──┐
────┘  └──┘  └──┘  └──

75% duty cycle (louder)
    ┌────┐ ┌────┐ ┌────┐
────┘    └─┘    └─┘    └─

Frequency = pitch (440Hz = A note)
Duty cycle = volume
```

**Code:**
```cpp
// Play 440Hz tone (A note) for 100ms
M5.Speaker.tone(440, 100);

// Internally:
ledcSetup(channel, 440, 8);     // 440Hz, 8-bit resolution
ledcAttachPin(GPIO2, channel);   // Connect to speaker pin
ledcWrite(channel, 128);         // 50% duty cycle
delay(100);
ledcWrite(channel, 0);           // Turn off
```

---

## Memory Architecture

### Flash Memory (4MB, Non-Volatile)

**Layout:**
```
0x00000000 ├─────────────────┐
           │  Bootloader     │ 32KB
0x00008000 ├─────────────────┤
           │  Partition Table│ 4KB
0x00009000 ├─────────────────┤
           │  NVS (EEPROM)   │ 16KB
0x0000D000 ├─────────────────┤
           │  OTA Data       │ 8KB
0x0000F000 ├─────────────────┤
           │  App0 (Code)    │ 1.3MB
0x00150000 ├─────────────────┤
           │  App1 (OTA)     │ 1.3MB
0x00290000 ├─────────────────┤
           │  SPIFFS/LittleFS│ 1.3MB
0x003D0000 └─────────────────┘
```

**EEPROM Emulation:**
- Actually stored in NVS (Non-Volatile Storage) partition
- Flash has ~100,000 write cycles
- Wear leveling distributes writes
- Batch writes to minimize wear

---

### RAM (320KB, Volatile)

**Allocation:**
```
┌─────────────────────┐ 0x3FFFFFFF
│  ROM (not usable)   │
├─────────────────────┤ 0x40000000
│  Instruction RAM    │ 128KB (code execution)
├─────────────────────┤
│  Data RAM (DRAM)    │ 192KB (variables, heap, stack)
│  ┌───────────────┐  │
│  │ Global vars   │  │ ← canvas, boyfriend, etc.
│  ├───────────────┤  │
│  │ Heap (grow ↓) │  │ ← malloc(), new
│  ├───────────────┤  │
│  │   (free)      │  │
│  ├───────────────┤  │
│  │ Stack (grow ↑)│  │ ← Local variables, function calls
│  └───────────────┘  │
└─────────────────────┘
```

**Memory Budget for This Project:**
- Canvas buffer: 135×240×2 = 64,800 bytes (~64KB)
- M5Unified library: ~20KB
- Your code global vars: ~5KB
- Stack (function calls): ~8KB
- **Total used: ~97KB / 192KB (50%)**

**Optimization Tips:**
```cpp
// ❌ Bad: Large buffer in RAM
uint8_t imageData[10000];  // 10KB of RAM!

// ✅ Good: Store in Flash
const uint8_t imageData[] PROGMEM = { ... };  // 0 bytes of RAM
uint8_t pixel = pgm_read_byte(&imageData[i]); // Read when needed

// ❌ Bad: String literals in RAM
char msg[] = "Hello World";  // Copies to RAM

// ✅ Good: Keep in Flash
const char msg[] = "Hello World";  // Stays in Flash
// or
Serial.println(F("Hello World"));  // F() macro = Flash storage
```

---

## Power Management

### Battery System

**Components:**
- 200mAh LiPo battery (3.7V nominal)
- AXP2101 PMIC (Power Management IC)
- USB-C charging port (5V input)

**Power States:**
```
┌─────────────────────────────────────────────┐
│ Active (Full Power)       │ ~150mA @ 3.7V  │
│ ├─ Display on             │   ~40mA        │
│ ├─ WiFi on                │   ~80mA        │
│ ├─ CPU running            │   ~30mA        │
│ └─ Peripherals            │   ~10mA        │
├───────────────────────────┼────────────────┤
│ Display Off               │  ~60mA         │
│ (WiFi still on)           │                │
├───────────────────────────┼────────────────┤
│ Light Sleep               │  ~10mA         │
│ (WiFi off, CPU halts)     │                │
├───────────────────────────┼────────────────┤
│ Deep Sleep                │  ~150µA        │
│ (Only RTC active)         │                │
└───────────────────────────┴────────────────┘

Runtime: 200mAh / 150mA = ~1.3 hours (full power)
         200mAh / 60mA = ~3.3 hours (display off)
```

**Reading Battery Level:**
```cpp
int batteryLevel = M5.Power.getBatteryLevel();  // 0-100%
bool isCharging = M5.Power.isCharging();
float voltage = M5.Power.getBatteryVoltage();   // 3.0-4.2V

// Voltage to percentage (rough approximation)
// 4.2V = 100%
// 3.7V = 50%
// 3.0V = 0%
```

---

## Sensors Deep Dive

### IMU (MPU6886) - 6-Axis Motion Sensor

**What It Measures:**

**Accelerometer:** Linear acceleration in 3 axes
- Measures gravity + movement
- Units: G-forces (1G = 9.8 m/s²)
- Range: ±2G (configurable to ±16G)

**Gyroscope:** Angular velocity in 3 axes
- Measures rotation speed
- Units: degrees/second
- Range: ±2000°/s

**Coordinate System:**
```
        Z (up)
        │
        │
        └─── Y (right)
       /
      /
     X (forward)

When device flat on table:
- accelX ≈ 0G  (no forward/back tilt)
- accelY ≈ 0G  (no left/right tilt)
- accelZ ≈ 1G  (gravity pulling down)

When device vertical (screen facing you):
- accelX ≈ 0G
- accelY ≈ -1G (gravity now on Y axis)
- accelZ ≈ 0G
```

**Common Patterns:**

**Detect "Flat":**
```cpp
bool isFlat = (abs(accelZ - 1.0) < 0.2);  // Z-axis ≈ 1G
```

**Detect "Tilt Up":**
```cpp
bool tiltedUp = (accelY < -0.7);  // Y-axis negative when tilted back
```

**Detect "Shake":**
```cpp
float totalAccel = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
if (totalAccel > 2.0) {  // More than 2G = shake
    shakeDetected = true;
}
```

**Calibration:**
- IMU has factory calibration
- Temperature affects readings (±0.02G drift)
- Continuous calibration possible (zero-motion detection)

---

### Microphone (SPM1423)

**Specifications:**
- PDM digital output
- Sensitivity: -26 dBFS
- SNR: 61 dB
- Frequency response: 100Hz - 10kHz

**Reading Microphone:**
```cpp
M5.Mic.begin();  // Initialize I2S interface

// In loop:
if (M5.Mic.isEnabled()) {
    int level = M5.Mic.getLevel();  // 0-255 amplitude
    if (level > 100) {  // Threshold for "loud"
        blowDetected = true;
    }
}
```

**How PDM Works:**
```
Analog Sound Wave:     PDM Bitstream:
    ╱╲                 11111110000111111000011111
   ╱  ╲               High density = high amplitude
  ╱    ╲╱             Low density = low amplitude
 ╱      ╲
```

---

## Display (ST7789)

**Specifications:**
- 135×240 pixels
- RGB565 color (16-bit)
- SPI interface @ 40MHz
- Refresh rate: ~60Hz

**Memory:**
```
Frame buffer: 135 × 240 × 2 bytes = 64,800 bytes

Why 2 bytes per pixel?
RGB565 format:
 15  11  10   5   4   0
 ┌────┬──────┬─────┐
 │ R  │  G   │  B  │
 │5bit│ 6bit │5bit │
 └────┴──────┴─────┘
 
Example: Red = 0b11111_000000_00000 = 0xF800
         Green = 0b00000_111111_00000 = 0x07E0
         Blue = 0b00000_000000_11111 = 0x001F
```

**Double Buffering:**
```
┌───────────┐      ┌───────────┐
│  Canvas   │      │  Display  │
│ (RAM)     │─────>│ (Screen)  │
│           │ Push │           │
│ Draw here │      │ Show here │
└───────────┘      └───────────┘

Benefits:
- No flicker (complete frame drawn before showing)
- Can draw complex graphics without visible tearing
```

---

## GPIO and Buttons

**Button Circuit:**
```
3.3V ───┐
        ├─ Pull-up resistor (10kΩ)
        │
GPIO37 ─┼───┐
            │
         [Button A]
            │
          GND
```

**How Buttons Work:**
- Idle: GPIO reads HIGH (3.3V) via pull-up
- Pressed: Button connects GPIO to GND → reads LOW (0V)

**Software Debouncing:**
```cpp
// Hardware bounce:
Press button → ┐ ┌┐ ┌┐┐┌─────  (noisy!)
               └─┘└─┘└┘└

// M5Unified handles this internally with state machine:
// - Detects press only after stable LOW for ~50ms
// - Provides wasPressed(), isPressed(), wasReleased()
```

---

## Timing and Interrupts

### System Timer

**millis() Function:**
- Returns milliseconds since boot
- 32-bit unsigned long: rolls over after 49.7 days
- Incremented by timer interrupt (every 1ms)

**FreeRTOS Ticks:**
- ESP32 runs FreeRTOS (Real-Time Operating System)
- Tick rate: 1kHz (1ms per tick)
- `millis()` returns tick count

**Safe Rollover Handling:**
```cpp
// ❌ Bad: Fails after 49 days
if (millis() > targetTime) { ... }

// ✅ Good: Works across rollover
if (millis() - startTime > duration) { ... }

// Why? Unsigned subtraction wraps correctly:
// If millis rolled over: 100 - 0xFFFFFFF0 = 100 + 16 = 116 ✓
```

---

## Building and Flashing

### PlatformIO Workflow

**1. Compile:**
```bash
~/.platformio/penv/bin/pio run
```

**What happens:**
```
1. Preprocessor: Expand #include, #define
2. Compiler: .cpp → .o (object files)
3. Linker: Combine .o files + libraries → .elf
4. Objcopy: .elf → .bin (binary for ESP32)
5. esptool: Create bootloader + partition table + app image
```

**2. Upload:**
```bash
~/.platformio/penv/bin/pio run -t upload
```

**What happens:**
```
1. Reset ESP32 into bootloader mode
2. Connect via USB serial (115200 baud)
3. Erase flash sectors
4. Write binary to flash @ 0x10000
5. Reset ESP32 → runs your code
```

**3. Monitor:**
```bash
~/.platformio/penv/bin/pio device monitor -b 115200
```

---

## Debugging Techniques

### 1. Serial Monitor (Primary Tool)

```cpp
Serial.begin(115200);
Serial.println("Debug message");
Serial.printf("Value: %d\n", variable);
```

### 2. LED Debugging

```cpp
digitalWrite(GPIO19, HIGH);  // Turn on red LED
delay(100);
digitalWrite(GPIO19, LOW);   // Turn off
```

### 3. On-Screen Debug

```cpp
canvas.setCursor(0, 0);
canvas.printf("State:%d AccZ:%.2f", state, accelZ);
```

### 4. GDB (Advanced)

- Requires JTAG debugger
- Can set breakpoints, step through code
- Beyond scope of this project

---

## Further Reading

**Datasheets:**
- [ESP32-PICO-V3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-pico-v3-02_datasheet_en.pdf)
- [MPU6886 IMU](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/MPU-6886-000193%2Bv1.1_GHIC_en.pdf)
- [ST7789 Display Driver](https://www.newhavendisplay.com/appnotes/datasheets/LCDs/ST7789V.pdf)

**M5Stack Docs:**
- [M5StickC Plus 2](https://docs.m5stack.com/en/core/M5StickC%20PLUS2)
- [M5Unified Library](https://github.com/m5stack/M5Unified)

**ESP32 Programming:**
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [Arduino-ESP32 Core](https://github.com/espressif/arduino-esp32)
