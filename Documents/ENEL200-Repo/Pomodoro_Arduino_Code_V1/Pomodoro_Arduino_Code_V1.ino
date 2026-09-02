#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

// -----Initialising pins-----
// Defining digital pins
// Button pins (input)
const int buttonPin1 = 11;
const int buttonPin2 = 12;
const int buttonPin3 = 13;

// Seven segment display transistor switches (output)
const int transPin1 = 10;
const int transPin2 = 9;

// Shift register pins (output)
const int shiftRegClock = 8; //CP - Clock pin D3 """
const int shiftRegData = 7; //DSB - serial input, D2 in instructables tutorial

// LED pins (output)
const int ledPin1 = 6;
const int ledPin2 = 5;
const int ledPin3 = 4;
const int ledPin4 = 3;

// Piezo buzzer pin (output)
const int buzzPin = 2;

// -----Variables-----

// Define button pin states;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

// LED variables
unsigned long previousMillisLED[4] = {0, 0, 0, 0};
int ledState[4] = {LOW, LOW, LOW, LOW};
int ledInterval = 500;

// millisecond to minute converter constant
const unsigned int millisecondsToMinutes = 60000;

// Pomodoro function global variables
unsigned long startPomoPeriodMillis;
unsigned long currentPomoPeriodMillis;
 
// Button logic variables 
const unsigned long HOLD_TIME = 2000;
const unsigned long DEBOUNCE_TIME = 50;

bool isTimerRunning = false;
bool inSettingsMenu = false;
int workPeriodMinutes = 25;
int displayValue = 25;

int buzzerSoundIndex = 1;

// 7 Segment display number map
const uint8_t segmentMap[10][8] = {
  {0, 1, 1, 1, 0, 1, 1, 1}, // 0
  {0, 0, 0, 1, 0, 1, 0, 0}, // 1
  {1, 0, 1, 1, 0, 0, 1, 1}, // 2
  {1, 0, 1, 1, 0, 1, 1, 0}, // 3
  {1, 1, 0, 1, 0, 1, 0, 0}, // 4
  {1, 1, 1, 0, 0, 1, 1, 0}, // 5
  {1, 1, 1, 0, 0, 1, 1, 1}, // 6
  {0, 0, 1, 1, 0, 1, 0, 0}, // 7
  {1, 1, 1, 1, 0, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 0, 0}  // 9
};

// Defines Pomodoro state for LED and buzzer
enum PomodoroState {
  WORK_1,  
  BREAK_1, 
  WORK_2,  
  BREAK_2, 
  WORK_3,  
  BREAK_3, 
  WORK_4,  
  BREAK_4, 
  IDLE     
};

// State Machine Tracker
PomodoroState cycle = WORK_1;

// --- Song Data ---
const char mission_imp[] PROGMEM = "Mission Imp:d=16,o=5,b=125:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p";
const char chariots_fire[] PROGMEM = "Chariots Fire:o=5,d=16,b=85,b=85:8c#,f#.,g#.,a#.,4g#,4f,8p,8c#,f#.,g#.,a#.,2g#,8p,8c#,f#.,g#.,a#.,4g#,4f,8p,8f,f#.,f.,c#.,2c#";
const char final_countdown[] PROGMEM = "Final Countdown:o=5,d=16,b=85,b=85:8c#,f#.,g#.,a#.,4g#,4f,8p,8c#,f#.,g#.,a#.,2g#,8p,8c#,f#.,g#.,a#.,4g#,4f,8p,8f,f#.,f.,c#.,2c#";
const char star_wars[] PROGMEM = "Star Wars:o=6,d=8,b=180,b=180:f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c,4p,f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c";
const char take_on_me[] PROGMEM = "Take On Me:o=5,d=8,b=160,b=160:f#,f#,f#,d,p,b4,p,e,p,e,p,e,g#,g#,a,b,a,a,a,e,p,d,p,f#,p,f#,p,f#,e,e,f#,e,f#,f#,f#,d,p,b4,p,e,p,e,p,e,g#,g#,a,b,a,a,a,e,p,d,p,f#,p,f#,p,f#,e,e5";
const char simple_signal[] PROGMEM = "Signal:d=16,o=6,b=180:c,p,c,g,p,g,4c7";

const char *ringtone = simple_signal;



void setup() {
  //----- Pin initialisation-----
  // initialise button pins
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT); 
  pinMode(buttonPin3, INPUT);

  // Initialise seven segment display transistor switches
  pinMode(transPin1, OUTPUT);
  pinMode(transPin2, OUTPUT);

  // Initialise shift register pins (output)
  pinMode(shiftRegClock, OUTPUT);
  pinMode(shiftRegData, OUTPUT);

  // Initialise LED pins
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

  // Initialise buzzer pin
  pinMode(buzzPin, OUTPUT);

  // Initialising PomodoroCycle Function
  startPomoPeriodMillis = millis();
}

void loop() {
  // Reading states of buttons
  button1();
  button2();
  button3();

  DisplayAndMultiplex(displayValue);    
}
// LED function - Done [Q]
// Pomodoro Cycle


// Work cycle
// break cycle
// Long break cycle
// Bell ringing function
// Bell mute function
// Changing soundtrack function
// changing time period
// converting time to tens and ones digits
// multiplexing function 74HC164
// button press function
void pomodoroCycle(unsigned long workPeriod, unsigned long shortBreakPeriod, unsigned long LongBreakPeriod, enum PomodoroState);

void onTimerStartUp()
{
  unsigned long workPeriod = 25 * millisecondsToMinutes;
  unsigned long shortBreakPeriod = 5 * millisecondsToMinutes;
  unsigned long LongBreakPeriod = 15 * millisecondsToMinutes;
  pomodoroCycle(workPeriod, shortBreakPeriod, LongBreakPeriod, cycle);
}

void ringTimer(const char *ringtone)
{
  static bool ringtoneDone = false;
  if (!ringtoneDone) {
    anyrtttl::nonblocking::beginProgMem(buzzPin, mission_imp);
    ringtoneDone = true;
  } else {
    anyrtttl::nonblocking::play();
  }  
}

void pomodoroCycle(unsigned long workPeriod, unsigned long shortBreakPeriod, unsigned long longBreakPeriod, enum PomodoroState) // When button two is on, and timer should be running.
{
  PomodoroState cycle = WORK_1;
  switch(cycle) {
    case WORK_1:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= workPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = BREAK_1;
      }
      break;
    case BREAK_1:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= shortBreakPeriod) { // Waiting for break period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = WORK_2;
      }
      break;
    case WORK_2:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= workPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = BREAK_2;
      }
      break;
    case BREAK_2:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= shortBreakPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = WORK_3;
      }
      break;
    case WORK_3:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= workPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = BREAK_3;
      }
      break;  
    case BREAK_3:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= shortBreakPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = WORK_4;
      }
      break;
    case WORK_4:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= workPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = BREAK_4;
      }
      break;
    case BREAK_4:
      updateLED(cycle, ledInterval);
      currentPomoPeriodMillis = millis();
      if (currentPomoPeriodMillis - startPomoPeriodMillis >= longBreakPeriod) { // Waiting for work period to end.
        startPomoPeriodMillis = currentPomoPeriodMillis;
        ringTimer(ringtone);
        PomodoroState cycle = WORK_1;
      }
      break;
    default:
    // Pause case?
    ringTimer(ringtone);
    PomodoroState cycle = WORK_1;
  }
}


void DisplayAndMultiplex(int valueToShow) 
{
  int tens = valueToShow / 10;
  int ones = valueToShow % 10;

  // --- Display Digit 1 (Tens) ---
  digitalWrite(transPin1, LOW);  // Ensure Ones digit is OFF
  shiftPattern(tens);                 // Send Tens bits to shift register
  digitalWrite(transPin2, HIGH); // Turn ON Tens digit
  delayMicroseconds(2000);            // Leave it on for 2ms

  // --- Display Digit 2 (Ones) ---
  digitalWrite(transPin1, LOW);  // Turn OFF Tens digit
  shiftPattern(ones);                 // Send Ones bits to shift register
  digitalWrite(transPin2, HIGH); // Turn ON Ones digit
  delayMicroseconds(2000);            // Leave it on for 2ms

}

void shiftPattern(uint8_t targetNum)
{
  for (int8_t i = 7; i >= 0; i--) {
    digitalWrite(shiftRegClock, LOW);
    digitalWrite(shiftRegData, segmentMap[targetNum][i]);
    digitalWrite(shiftRegClock, HIGH);
  }
}

void button1()
{
  static bool isPressing = false;
  static bool hold = false;
  static unsigned long pressedTime = 0;
  static unsigned long releasedTime = 0;

  unsigned long pressDuration = 0;
  unsigned long totalPressDuration = 0;

  buttonState1 = digitalRead(buttonPin1);

  if (buttonState1 == HIGH && !isPressing) {
    pressedTime = millis();
    isPressing = true;
    hold = false;
  }
  
  if (buttonState1 == HIGH && isPressing && !hold) {
    pressDuration = millis() - pressedTime;
    if (pressDuration >= HOLD_TIME) {
      // Button 1 Hold function
      hold = true;
    }
  }

  if (buttonState1 == LOW && isPressing) {
    releasedTime = millis();
    totalPressDuration = releasedTime - pressedTime;
    isPressing = false;
    if (totalPressDuration > DEBOUNCE_TIME && !hold) {
      // Button 1 Press function
      buzzerSoundIndex++;
      if (buzzerSoundIndex > 3) {
        buzzerSoundIndex = 0;
      }

      if (buzzerSoundIndex != 0) {
        tone(buzzPin, 1000 + (buzzerSoundIndex * 200), 50);
      }
    }
  } 
}

void button2()
{
  static bool isPressing = false;
  static bool hold = false;
  static unsigned long pressedTime = 0;
  static unsigned long releasedTime = 0;

  unsigned long pressDuration = 0;
  unsigned long totalPressDuration = 0;

  buttonState2 = digitalRead(buttonPin2);

  if (buttonState2 == HIGH && !isPressing) {
    pressedTime = millis();
    isPressing = true;
    hold = false;
  }

  if (buttonState2 == HIGH && isPressing && !hold) {
    pressDuration = millis() - pressedTime;
    
    if (pressDuration >= HOLD_TIME) {
      // Button 2 Hold function
      hold = true;
      isTimerRunning = false;
      displayValue = 25;
      startPomoPeriodMillis = millis();

      tone(buzzPin, 800, 100);
      delay(150);
      tone(buzzPin, 800, 100);

    }
  }

  if (buttonState2 == LOW && isPressing) {
    releasedTime = millis();
    totalPressDuration = releasedTime - pressedTime;
    isPressing = false;

    if (totalPressDuration > DEBOUNCE_TIME && !hold) {
      // Button 2 Press function
      isTimerRunning = !isTimerRunning;

      if (buzzerSoundIndex != 0) {
        tone(buzzPin, isTimerRunning ? 1500 : 700, 80);
      }
    }
  }
}


void button3()
{
  static bool isPressing = false;
  static bool hold = false;
  static unsigned long pressedTime = 0;
  static unsigned long releasedTime = 0;

  unsigned long pressDuration = 0;
  unsigned long totalPressDuration = 0;

  buttonState3 = digitalRead(buttonPin3);

  if (buttonState3 == HIGH && !isPressing) {
    pressedTime = millis();
    isPressing = true;
    hold = false;
  }
  if (buttonState3 == HIGH && isPressing && !hold) {
    pressDuration = millis() - pressedTime;

    if (pressDuration >= HOLD_TIME) {
      // Button 3 Hold function
      hold = true;
    }
  }

  if (buttonState3 == LOW && isPressing) {
    releasedTime = millis();
    long totalPressDuration = releasedTime - pressedTime;

    isPressing = false;

    if (totalPressDuration > DEBOUNCE_TIME && !hold) {
      
      cycle = (PomodoroState)((cycle + 1) % 8);
      startPomoPeriodMillis = millis();
    }
  } 
}


// LED function - Done [Q]

void turnOnLED(int thisLed)
{
  digitalWrite(thisLed, HIGH);
}

void turnOffLED(int thisLed) {
  digitalWrite(thisLed, LOW);
}

void blinkLed(int led_index, int thisLed, int ledInterval) // 10 - LED light-up function, takes led number and desired flash interval
// How do we make this only flash for period of break? If statement in break.
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisLED[led_index] >= ledInterval) {
    previousMillisLED[led_index] = currentMillis;
    
    if (ledState[led_index] == LOW) {
      ledState[led_index] = HIGH;
    } else {
      ledState[led_index] = LOW;
    }
    digitalWrite(thisLed, ledState[led_index]);
  }
}


// updateLED helper function 

void updateLED(PomodoroState current_state, int ledInterval)
{
  int led_index = -1;
  bool isBlinkingPhase = false;

  switch(current_state) {
    case WORK_1:
      turnOnLED(ledPin1);   
      turnOffLED(ledPin2);  
      turnOffLED(ledPin3);
      turnOffLED(ledPin4);
      break;
    case BREAK_1:
      blinkLed(0, ledPin1, ledInterval); 
      turnOffLED(ledPin2);               
      turnOffLED(ledPin3);
      turnOffLED(ledPin4);
      break;
    case WORK_2:
      turnOnLED(ledPin1);   
      turnOnLED(ledPin2);  
      turnOffLED(ledPin3);
      turnOffLED(ledPin4);
      break;
    case BREAK_2:
      blinkLed(0, ledPin1, ledInterval); 
      blinkLed(1, ledPin2, ledInterval);           
      turnOffLED(ledPin3);
      turnOffLED(ledPin4);
      break;
    case WORK_3:
      turnOnLED(ledPin1);   
      turnOnLED(ledPin2);  
      turnOnLED(ledPin3);
      turnOffLED(ledPin4);
      break;
    case BREAK_3:
      blinkLed(0, ledPin1, ledInterval); 
      blinkLed(1, ledPin2, ledInterval);           
      blinkLed(2, ledPin3, ledInterval); 
      turnOffLED(ledPin4);
      break;

    case WORK_4:
      turnOnLED(ledPin1);   
      turnOnLED(ledPin2);  
      turnOnLED(ledPin3);
      turnOnLED(ledPin4);
      break;
    case BREAK_4:
      blinkLed(0, ledPin1, ledInterval); 
      blinkLed(1, ledPin2, ledInterval);           
      blinkLed(2, ledPin3, ledInterval); 
      blinkLed(3, ledPin4, ledInterval); 
      break;

    case IDLE:
      turnOffLED(ledPin1);
      turnOffLED(ledPin2);
      turnOffLED(ledPin3);
      turnOffLED(ledPin4);
      break;

  }
  
}
