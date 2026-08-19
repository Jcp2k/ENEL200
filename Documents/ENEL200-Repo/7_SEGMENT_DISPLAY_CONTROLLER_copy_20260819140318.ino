/* Pomodoro Timer System */

// ----------- Shift Registor -----------
const uint8_t PIN_SHIFT_DATA  = 7;
const uint8_t PIN_SHIFT_CLOCK = 8;

// ----------- Mosfets -----------
const uint8_t PIN_DIGIT1 = 9;
const uint8_t PIN_DIGIT2 = 10;


// ----------- Buttons -----------
const uint8_t PIN_BTN1 = 11;
const uint8_t PIN_BTN2 = 12;
const uint8_t PIN_BTN3 = 13;

// LEDs
const uint8_t PIN_LED[4] = {3, 4, 5, 6};

// Buzzer
const uint8_t PIN_BUZZER = 2;

// ----------- Editable Durations (Minutes) -----------
uint8_t workMinutes = 25;
uint8_t shortBreakMinutes = 5;
uint8_t longBreakMinutes = 15;

// ----------- 7-Segment Display digit patterns -----------
const uint8_t segmentMap[10][8] = {
  {0, 1, 1, 1, 1, 1, 1, 1}, // 0
  {0, 0, 0, 1, 0, 1, 0, 0}, // 1
  {1, 0, 1, 1, 0, 0, 1, 1}, // 2
  {1, 0, 1, 1, 0, 1, 1, 0}, // 3
  {1, 1, 0, 1, 0, 1, 0, 0}, // 4
  {1, 1, 1, 0, 0, 1, 1, 0}, // 5
  {1, 1, 1, 0, 0, 1, 1, 1}, // 6
  {0, 0, 1, 1, 0, 1, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 0, 0}  // 9
};

// ----------- App Mode -----------
enum AppMode {MODE_NORMAL, MODE_SOUND_CONFIG,MODE_TIME_CONFIG};
AppMode appMode = MODE_NORMAL;

// ----------- Timer State Machine -----------
enum RunState {STOPPED, RUNNING};
RunState runState = STOPPED;

bool isBreak = false;
uint8_t session = 1;

uint32_t secondsRemaining = 0;
uint32_t lastTickMillis = 0;

// ----------- Time config editing state -----------
enum ConfigTarget {CFG_WORK, CFG_SHORT_BREAK, CFG_LONG_BREAK};
ConfigTarget configTarget = CFG_WORK;
uint8_t selectedDigit = 0;

// ----------- LED Blink -----------
bool blinkOn = true;
uint32_t lastBlinkToggle = 0;
const uint32_t BLINK_INTERVAL_MS = 500;

// ----------- Digit select blink (time config cursor) -----------
bool cursorBlinkOn = true;
uint32_t lastCursorToggle = 0;
const uint32_t CURSOR_BLINK_INTERVAL_MS = 300;

// ---------- Digit multiplexing ----------
uint32_t lastMuxSwitch = 0;
const uint32_t MUX_INTERVAL_MS = 4;
bool showingDigit1 = true;

// ---------- Buzzer (manual square wave for volume control) ----------
bool buzzerActive = false;
uint32_t buzzStopAtMs = 0;
uint32_t buzzPeriodUs = 0;
uint32_t buzzOnUs = 0;
uint32_t buzzCycleStartUs = 0;

// ---------- Button handling ----------
enum ButtonEvent { EVT_NONE, EVT_PUSH, EVT_HOLD };

struct Button {
  uint8_t pin;
  bool rawLast;
  bool stableState;
  uint32_t lastChangeMs;
  uint32_t pressStartMs;
  bool holdFired;
};
Button btn1 = {PIN_BTN1, LOW, LOW, 0, 0, false};
Button btn2 = {PIN_BTN2, LOW, LOW, 0, 0, false};
Button btn3 = {PIN_BTN3, LOW, LOW, 0, 0, false};










int8_t displayValue = 0;
bool lastButtonState = LOW; // Default LOW due to external pull-down

void displayDigit(uint8_t targetNum) {
  // Your exact custom loop timing logic preserved
  for (int8_t i = 7; i >= 0; i--) {
    digitalWrite(PIN_SHIFT_CLOCK, LOW);
    // Directly reads index 7 down to 0 to push onto the shift register
    digitalWrite(PIN_SHIFT_DATA, segmentMap[targetNum][i]);
    digitalWrite(PIN_SHIFT_CLOCK, HIGH);
  }
}

void setup() {
  pinMode(PIN_SHIFT_DATA, OUTPUT);
  pinMode(PIN_SHIFT_CLOCK, OUTPUT);
  pinMode(PIN_DIGIT, OUTPUT);
  
  pinMode(PIN_BUTTON, INPUT); // Handled by external pull-down resistor
  
  digitalWrite(PIN_DIGIT, HIGH); // Enable the display digit
  
  // Show '0' immediately at startup
  displayDigit(displayValue);
}

void loop() {
  bool currentButtonState = digitalRead(PIN_BUTTON);

  // Detect state transition from unpressed (LOW) to pressed (HIGH)
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    displayValue++;
    if (displayValue > 9) {
      displayValue = 0; // Cycle back to 0 after reaching 9
    }
    
    // Update the display with the new digit
    displayDigit(displayValue);
    
    // Debounce delay to prevent a single press counting multiple times
    delay(150);
  }
  
  lastButtonState = currentButtonState;
}
