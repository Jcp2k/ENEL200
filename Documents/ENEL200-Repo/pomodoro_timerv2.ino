/*
  Arduino Pomodoro Timer - button-driven config menus
  ------------------------------------------------------------
  Hardware (current prototype wiring):
    - 1x 74HC164 shift register driving the SEGMENTS (a-g).
      Unlike the 74HC595 there is NO latch pin - bits appear on
      the outputs as they're clocked in, so the digits are
      blanked while a new pattern shifts through to avoid a
      visible "ripple".
    - 2N7000 MOSFETs select which digit is currently lit
      (gate driven directly by an Arduino pin). Only one display
      is wired up right now (pin 9); a second one will go on
      pin 10 later - the code already multiplexes both, the
      second digit just won't light until it's connected.
    - 3 push buttons, each with a pull-down resistor
        (button pressed = HIGH)
    - 4 LEDs showing the current pomodoro count
    - 1 piezo buzzer, software square-wave generated so volume
      (duty cycle) can be controlled from the buttons

  ------------------------------------------------------------
  BUTTON BEHAVIOUR (see flowchart)
  ------------------------------------------------------------
  Button 1
    PUSH  -> Volume up by 10%            (wraps 100 -> 0)
    HOLD  -> Enter Sound Config
               PUSH  -> Cycle Sounds (plays a preview)
               HOLD  -> Exit back to normal mode

  Button 2
    PUSH  -> Start timer / Stop timer (toggle)
    HOLD  -> Reset current cycle (current work/break segment
             only - session count and mode are unchanged)

  Button 3
    PUSH  -> Skip current cycle
    HOLD  -> Enter Time Config
               PUSH 2 (Button 2) -> Cycle Work / Short Break / Long Break
               PUSH 1 (Button 1) -> Increase selected digit by 1
               PUSH 3 (Button 3) -> Change which digit is selected
               HOLD   (Button 3) -> Exit, saving the new durations

  Finishing the 4th (long) break automatically wraps the whole
  cycle back to Work 1 and keeps running.
*/

#include <Arduino.h>

// ---------- Pin assignments (current prototype wiring) ----------
// 74HC164 shift register (segments) - serial-in, no latch pin
const uint8_t PIN_SHIFT_DATA  = 7; // DSB
const uint8_t PIN_SHIFT_CLOCK = 8; // CP

// Digit select MOSFETs (2N7000 gates)
const uint8_t PIN_DIGIT1 = 9;  // currently-wired display
const uint8_t PIN_DIGIT2 = 10; // second display - not wired yet

// Buttons (pull-down resistors -> pressed reads HIGH)
const uint8_t PIN_BTN1 = 11; // Volume / Sound Config
const uint8_t PIN_BTN2 = 12; // Start-Stop / Reset current cycle
const uint8_t PIN_BTN3 = 13; // Skip / Time Config

// LEDs
const uint8_t PIN_LED[4] = {3, 4, 5, 6};

// Buzzer - must be a normal digital pin, driven manually (not tone())
const uint8_t PIN_BUZZER = 2;

// ---------- Polarity (flip if your hardware is wired opposite) ----------
const bool SEG_ON   = true; // shift register bit level that lights a segment
const bool DIGIT_ON = HIGH; // digit-select pin level that lights that digit

// ---------- Timing thresholds ----------
const uint32_t DEBOUNCE_MS = 30;
const uint32_t HOLD_THRESHOLD_MS = 600; // how long counts as a HOLD

// ---------- Editable durations (minutes) ----------
uint8_t workMinutes = 25;
uint8_t shortBreakMinutes = 5;
uint8_t longBreakMinutes  = 15;

// ---------- Sound options for state-change / config beeps ----------
const uint16_t SOUND_FREQS[] = {880, 1046, 1318, 660, 1568};
const uint8_t NUM_SOUNDS = sizeof(SOUND_FREQS) / sizeof(SOUND_FREQS[0]);
uint8_t soundIndex = 0;

// ---------- Volume (0-100%, duty-cycle based) ----------
uint8_t buzzVolume = 50;

// ---------- 7-seg digit patterns, bit order gfedcba (bit0=a ... bit6=g) ----------
const uint8_t DIGIT_PATTERNS[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};
const uint8_t DIGIT_BLANK = 0b0000000;

// ---------- App mode ----------
enum AppMode { MODE_NORMAL, MODE_SOUND_CONFIG, MODE_TIME_CONFIG };
AppMode appMode = MODE_NORMAL;

// ---------- Timer state machine ----------
enum RunState { STOPPED, RUNNING };
RunState runState = STOPPED;

bool isBreak = false;     // false = work, true = break
uint8_t session = 1;      // 1-4

uint32_t secondsRemaining = 0;
uint32_t lastTickMillis = 0;

// ---------- Time config editing state ----------
enum ConfigTarget { CFG_WORK, CFG_SHORT_BREAK, CFG_LONG_BREAK };
ConfigTarget configTarget = CFG_WORK;
uint8_t selectedDigit = 0; // 0 = tens of minutes, 1 = ones of minutes

// ---------- LED blink (break states) ----------
bool blinkOn = true;
uint32_t lastBlinkToggle = 0;
const uint32_t BLINK_INTERVAL_MS = 500;

// ---------- Digit-select blink (time config cursor) ----------
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

// ---------- Forward declarations ----------
ButtonEvent pollButton(Button &b);
void writeShiftRegister(uint8_t value);
void refreshDisplayMux();
uint32_t currentStateDurationSeconds();
void enterNextState();
void tickClock();
void updateLEDs();
void startBuzz(uint16_t freq, uint16_t durationMs);
void playNotification(uint16_t durationMs);
void playClick();
void serviceBuzzer();
uint8_t *targetMinutePtr();
void volumeUp();
void enterSoundConfig();
void exitSoundConfig();
void cycleSounds();
void toggleStartStop();
void resetCurrentCycle();
void enterTimeConfig();
void exitTimeConfig();
void cycleConfigTarget();
void incrementSelectedDigit();
void changeDigitCursor();

void setup() {
  pinMode(PIN_SHIFT_DATA, OUTPUT);
  pinMode(PIN_SHIFT_CLOCK, OUTPUT);
  pinMode(PIN_DIGIT1, OUTPUT);
  pinMode(PIN_DIGIT2, OUTPUT);

  pinMode(PIN_BTN1, INPUT); // external pull-down already on board
  pinMode(PIN_BTN2, INPUT);
  pinMode(PIN_BTN3, INPUT);

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(PIN_LED[i], OUTPUT);
    digitalWrite(PIN_LED[i], LOW);
  }

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  secondsRemaining = currentStateDurationSeconds();
  lastTickMillis = millis();
  updateLEDs();
}

void loop() {
  ButtonEvent e1 = pollButton(btn1);
  ButtonEvent e2 = pollButton(btn2);
  ButtonEvent e3 = pollButton(btn3);

  switch (appMode) {
    case MODE_NORMAL:
      if (e1 == EVT_PUSH) volumeUp();
      if (e1 == EVT_HOLD) enterSoundConfig();
      if (e2 == EVT_PUSH) toggleStartStop();
      if (e2 == EVT_HOLD) resetCurrentCycle();
      if (e3 == EVT_PUSH) enterNextState();
      if (e3 == EVT_HOLD) enterTimeConfig();
      break;

    case MODE_SOUND_CONFIG:
      if (e1 == EVT_PUSH) cycleSounds();
      if (e1 == EVT_HOLD) exitSoundConfig();
      break;

    case MODE_TIME_CONFIG:
      if (e2 == EVT_PUSH) cycleConfigTarget();
      if (e1 == EVT_PUSH) incrementSelectedDigit();
      if (e3 == EVT_PUSH) changeDigitCursor();
      if (e3 == EVT_HOLD) exitTimeConfig();
      break;
  }

  if (runState == RUNNING && appMode == MODE_NORMAL) {
    tickClock();
  }

  updateLEDs();
  refreshDisplayMux();
  serviceBuzzer();
}

// ================= Button polling (push vs hold) =================
ButtonEvent pollButton(Button &b) {
  bool raw = digitalRead(b.pin);
  uint32_t now = millis();

  if (raw != b.rawLast) {
    b.lastChangeMs = now;
    b.rawLast = raw;
  }

  if ((now - b.lastChangeMs) > DEBOUNCE_MS && raw != b.stableState) {
    b.stableState = raw;
    if (b.stableState == HIGH) {
      b.pressStartMs = now;
      b.holdFired = false;
    } else if (!b.holdFired) {
      return EVT_PUSH; // released before hold threshold
    }
  }

  if (b.stableState == HIGH && !b.holdFired &&
      (now - b.pressStartMs) >= HOLD_THRESHOLD_MS) {
    b.holdFired = true;
    return EVT_HOLD;
  }

  return EVT_NONE;
}

// ================= Normal-mode actions =================
void volumeUp() {
  buzzVolume += 10;
  if (buzzVolume > 100) buzzVolume = 0;
  playClick();
}

void toggleStartStop() {
  if (runState == RUNNING) {
    runState = STOPPED;
  } else {
    runState = RUNNING;
    lastTickMillis = millis(); // avoid a big jump on resume
  }
  playClick();
}

void resetCurrentCycle() {
  secondsRemaining = currentStateDurationSeconds();
  lastTickMillis = millis();
  playClick();
}

// ================= Sound config =================
void enterSoundConfig() {
  appMode = MODE_SOUND_CONFIG;
  runState = STOPPED; // pause while configuring
  playClick();
}

void exitSoundConfig() {
  appMode = MODE_NORMAL;
  playClick();
}

void cycleSounds() {
  soundIndex = (soundIndex + 1) % NUM_SOUNDS;
  playNotification(150); // preview the newly selected sound
}

// ================= Time config =================
void enterTimeConfig() {
  appMode = MODE_TIME_CONFIG;
  runState = STOPPED; // pause while configuring
  configTarget = CFG_WORK;
  selectedDigit = 0;
  playClick();
}

void exitTimeConfig() {
  appMode = MODE_NORMAL;
  // if the state currently on screen matches what we just edited,
  // refresh the countdown so the new duration takes effect immediately
  bool editingCurrentState =
      (!isBreak && configTarget == CFG_WORK) ||
      (isBreak && session != 4 && configTarget == CFG_SHORT_BREAK) ||
      (isBreak && session == 4 && configTarget == CFG_LONG_BREAK);
  if (editingCurrentState) {
    secondsRemaining = currentStateDurationSeconds();
  }
  playClick();
}

void cycleConfigTarget() {
  configTarget = (ConfigTarget)((configTarget + 1) % 3);
  selectedDigit = 0;
  playClick();
}

uint8_t *targetMinutePtr() {
  switch (configTarget) {
    case CFG_WORK:         return &workMinutes;
    case CFG_SHORT_BREAK:  return &shortBreakMinutes;
    default:                return &longBreakMinutes;
  }
}

void incrementSelectedDigit() {
  uint8_t *minutes = targetMinutePtr();
  uint8_t tens = (*minutes / 10) % 10;
  uint8_t ones = *minutes % 10;

  if (selectedDigit == 0) {
    tens = (tens + 1) % 10;
  } else {
    ones = (ones + 1) % 10;
  }

  uint8_t newValue = tens * 10 + ones;
  *minutes = (newValue == 0) ? 1 : newValue; // don't allow a 0-minute duration
  playClick();
}

void changeDigitCursor() {
  selectedDigit = (selectedDigit + 1) % 2;
  playClick();
}

// ================= Countdown / state machine =================
uint32_t currentStateDurationSeconds() {
  if (!isBreak) return (uint32_t)workMinutes * 60UL;
  return (session == 4)
             ? (uint32_t)longBreakMinutes * 60UL
             : (uint32_t)shortBreakMinutes * 60UL;
}

void tickClock() {
  uint32_t now = millis();
  if (now - lastTickMillis >= 1000) {
    uint32_t elapsedSeconds = (now - lastTickMillis) / 1000;
    lastTickMillis += elapsedSeconds * 1000;

    if (elapsedSeconds >= secondsRemaining) {
      secondsRemaining = 0;
      enterNextState();
    } else {
      secondsRemaining -= elapsedSeconds;
    }
  }
}

void enterNextState() {
  if (!isBreak) {
    isBreak = true;
    secondsRemaining = currentStateDurationSeconds();
  } else if (session >= 4) {
    // just finished the LONG break -> reset the whole cycle, keep running
    session = 1;
    isBreak = false;
    secondsRemaining = currentStateDurationSeconds();
  } else {
    session++;
    isBreak = false;
    secondsRemaining = currentStateDurationSeconds();
  }
  lastTickMillis = millis();
  playNotification(200);
}

// ================= LEDs =================
void updateLEDs() {
  if (!isBreak) {
    for (uint8_t i = 0; i < 4; i++) {
      digitalWrite(PIN_LED[i], (i < session) ? HIGH : LOW);
    }
    return;
  }

  if (millis() - lastBlinkToggle >= BLINK_INTERVAL_MS) {
    lastBlinkToggle = millis();
    blinkOn = !blinkOn;
  }
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(PIN_LED[i], (i < session && blinkOn) ? HIGH : LOW);
  }
}

// ================= Buzzer (manual square wave, volume = duty cycle) =================
void startBuzz(uint16_t freq, uint16_t durationMs) {
  if (buzzVolume == 0 || freq == 0) return;
  buzzerActive = true;
  buzzStopAtMs = millis() + durationMs;
  buzzPeriodUs = 1000000UL / freq;
  uint8_t vol = buzzVolume < 5 ? 5 : buzzVolume; // audible floor
  buzzOnUs = (buzzPeriodUs * vol) / 100UL;
  buzzCycleStartUs = micros();
}

void playNotification(uint16_t durationMs) {
  startBuzz(SOUND_FREQS[soundIndex], durationMs);
}

void playClick() {
  startBuzz(2000, 40); // short, fixed-pitch UI feedback tone
}

void serviceBuzzer() {
  if (!buzzerActive) {
    digitalWrite(PIN_BUZZER, LOW);
    return;
  }
  if (millis() >= buzzStopAtMs) {
    buzzerActive = false;
    digitalWrite(PIN_BUZZER, LOW);
    return;
  }
  uint32_t elapsed = micros() - buzzCycleStartUs;
  if (elapsed >= buzzPeriodUs) {
    buzzCycleStartUs += buzzPeriodUs;
    elapsed = micros() - buzzCycleStartUs;
  }
  digitalWrite(PIN_BUZZER, elapsed < buzzOnUs ? HIGH : LOW);
}

// ================= 7-seg display via shift register =================
// 74HC164 has no latch: bits appear on the outputs the instant they're
// clocked in. The caller is responsible for blanking both digits before
// calling this, so the mid-shift "ripple" never actually lights up.
void writeShiftRegister(uint8_t value) {
  uint8_t bits = SEG_ON ? value : (uint8_t)~value;
  for (int8_t i = 7; i >= 0; i--) {           // MSB first, matches old 74HC595 order
    digitalWrite(PIN_SHIFT_CLOCK, LOW);
    digitalWrite(PIN_SHIFT_DATA, (bits >> i) & 0x01);
    digitalWrite(PIN_SHIFT_CLOCK, HIGH);       // 164 shifts in on the rising edge
  }
}

void refreshDisplayMux() {
  if (millis() - lastMuxSwitch < MUX_INTERVAL_MS) return;
  lastMuxSwitch = millis();

  uint8_t tensPattern, onesPattern;

  if (appMode == MODE_SOUND_CONFIG) {
    // show the sound number (1-based) on the ones digit
    tensPattern = DIGIT_BLANK;
    onesPattern = DIGIT_PATTERNS[(soundIndex + 1) % 10];

  } else if (appMode == MODE_TIME_CONFIG) {
    if (millis() - lastCursorToggle >= CURSOR_BLINK_INTERVAL_MS) {
      lastCursorToggle = millis();
      cursorBlinkOn = !cursorBlinkOn;
    }
    uint8_t minutes = *targetMinutePtr();
    uint8_t tens = (minutes / 10) % 10;
    uint8_t ones = minutes % 10;

    tensPattern = DIGIT_PATTERNS[tens];
    onesPattern = DIGIT_PATTERNS[ones];

    // blank out the selected digit while the cursor blink is "off"
    if (selectedDigit == 0 && !cursorBlinkOn) tensPattern = DIGIT_BLANK;
    if (selectedDigit == 1 && !cursorBlinkOn) onesPattern = DIGIT_BLANK;

  } else {
    // normal countdown: MM while >1 minute left, SS for the final minute
    uint16_t minutesLeft = secondsRemaining / 60;
    uint16_t secondsLeft = secondsRemaining % 60;
    uint8_t tens, ones;
    if (minutesLeft > 0) {
      tens = (minutesLeft / 10) % 10;
      ones = minutesLeft % 10;
    } else {
      tens = (secondsLeft / 10) % 10;
      ones = secondsLeft % 10;
    }
    tensPattern = DIGIT_PATTERNS[tens];
    onesPattern = DIGIT_PATTERNS[ones];
  }

  digitalWrite(PIN_DIGIT1, !DIGIT_ON);
  digitalWrite(PIN_DIGIT2, !DIGIT_ON);

  if (showingDigit1) {
    writeShiftRegister(tensPattern);
    digitalWrite(PIN_DIGIT1, DIGIT_ON);
  } else {
    writeShiftRegister(onesPattern);
    digitalWrite(PIN_DIGIT2, DIGIT_ON);
  }
  showingDigit1 = !showingDigit1;
}
