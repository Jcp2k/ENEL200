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

// 10 - LED variables
unsigned long previousMillisLED = 0;
int ledState = LOW;
int ledinterval = 500; 

// millisecond to minute converter constant;

const unsigned int millisecondsToMinutes = 60000;
// Pomodoro function global variables

unsigned long startPomoPeriodMillis;
unsigned long currentPomoPeriodMillis;
 

// Button logic variables 

const unsigned long HOLD_TIME = 2000;
const unsigned long DEBOUNCE_TIME = 50;


// 9 - Shift register variables
// binary notation for display numbers
byte zero  = B01111110;
byte one   = B00000110;
byte two   = B11011010;
byte three = B11010110;
byte four  = B10100110;
byte five  = B11110100;
byte six   = B11111100;
byte seven = B01000110;
byte eight = B11111110;
byte nine  = B11110110;

// Defines Pomdoro State for LED and Buzzer

enum PomodoroState {
  WORK_1,  
  BREAK_1, 
  WORK_2,  
  BREAK_2, 
  WORK_3,  
  BREAK_3, 
  WORK_4,  
  BREAK_4, 
  IDLE   // Redundant??  
};

// --- Song Data ---
const char mission_imp[] PROGMEM = "Mission Imp:d=16,o=5,b=125:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c6,p,g,8p,g,8p,f,p,f#,p";
const char chariots_fire[] PROGMEM = "Chariots Fire:o=5,d=16,b=85,b=85:8c#,f#.,g#.,a#.,4g#,4f,8p,8c#,f#.,g#.,a#.,2g#,8p,8c#,f#.,g#.,a#.,4g#,4f,8p,8f,f#.,f.,c#.,2c#";
const char final_countdown[] PROGMEM = "Final Countdown:o=5,d=16,b=85,b=85:8c#,f#.,g#.,a#.,4g#,4f,8p,8c#,f#.,g#.,a#.,2g#,8p,8c#,f#.,g#.,a#.,4g#,4f,8p,8f,f#.,f.,c#.,2c#";
const char star_wars[] PROGMEM = "Star Wars:o=6,d=8,b=180,b=180:f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c,4p,f5,f5,f5,2a#5.,2f.,d#,d,c,2a#.,4f.,d#,d,c,2a#.,4f.,d#,d,d#,2c";
const char take_on_me[] PROGMEM = "Take On Me:o=5,d=8,b=160,b=160:f#,f#,f#,d,p,b4,p,e,p,e,p,e,g#,g#,a,b,a,a,a,e,p,d,p,f#,p,f#,p,f#,e,e,f#,e,f#,f#,f#,d,p,b4,p,e,p,e,p,e,g#,g#,a,b,a,a,a,e,p,d,p,f#,p,f#,p,f#,e,e5";
const char simple_signal[] PROGMEM = "Signal:d=16,o=6,b=180:c,p,c,g,p,g,4c7";
char ringtone[] = simple_signal; 



void setup() {
  //----- Pin initialisation-----
  // initialise button pins
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT); 
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);

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
  pinMode(buzzPin, OUPUT);

// Initialising PomodoroCycle Function
startPomoPeriodMillis = millis();

}

void loop() {
  // Reading states of buttons
  button1();
  button2();
  button3();  
}
// Bell ringing function - Done - Callum
// multiplexing function 74HC164 - Done - Jackson
// LED function - Done - Callum - [Have improvements]
// Pomodoro Cycle - Done - Callum - [Have improvements]
  // Work cycle
  // break cycle
  // Long break cycle 


// git status --> red
// git add .
// 


// Bell mute function
// Changing soundtrack function
// changing time period
// converting time to tens and ones digits
// button press function

//
void onTimerStartUp()
{
  unsigned long workPeriod = 25 * millisecondsToMinutes;
  unsigned long shortBreakPeriod = 5 * millisecondsToMinutes;
  unsigned long LongBreakPeriod = 15 * millisecondsToMinutes;
  pomodoroCycle(workPeriod, shortBreakPeriod, LongBreakPeriod, \isButton2paused\);
}


void ringTimer(ringtone)
{
  static bool ringtoneDone = false;
  if (!ringtoneDone) {
    anyrtttl::nonblocking::beginProgMem(BUZZER_PIN, mission_imp);
    ringtoneDone = true;
  } else {
    anyrtttl::nonblocking::play();
  }  
}


// ledInterval as a bool just so we know if want LEDs blinking or not, since interval won't change.
void pomodoroCycle(unsigned long workPeriod, unsigned long shortBreakPeriod, unsigned long LongBreakPeriod, enum pomodoroState, \isButton2paused\) // When button two is on, and timer should be running.
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

void DisplayAndMultiplex()
//Turn off both MOSFETs (blank)
//Shift in digit 1's 7-bit segment pattern
//Turn on digit 1's MOSFET, hold ~1-5 ms
//Turn off digit 1's MOSFET
//Shift in digit 2's 7-bit segment pattern
//Turn on digit 2's MOSFET, hold ~1-5 ms
//Loop



void button1();
{

  bool isPressing = false;
  bool hold = false;


  buttonState1 = digitalRead(buttonPin1);

  if (buttonState1 == LOW && !isPressing) {
    pressedTime = millis()
    isPressing = true;
    hold = false;
  }
  if (buttonstate1 == LOW && isPressing && !hold) {
    long pressDuration = millis() - pressedTime;
  } if (pressDuration >= HOLD_TIME) {
    // Button 1 Hold function
    hold = true;
  }

  if (buttonState1 == HIGH && isPressing) {
    releasedTime = millis()
    long totalPressDuration = releasedTime - pressedTime;
    isPressing = false;
  } if (totalPressDuration > DEBOUNCE_TIME && !hold) {
    // Button 1 Press function
  }
}

void button2();
{

  bool isPressing = false;
  bool hold = false;

  buttonState2 = digitalRead(buttonPin1);

  if (buttonState2 == LOW && !isPressing) {
    pressedTime = millis()
    isPressing = true;
    hold = false;
  }
  if (buttonstate2 == LOW && isPressing && !hold) {
    long pressDuration = millis() - pressedTime;
  } if (pressDuration >= HOLD_TIME) {
    // Button 2 Hold function
    hold = true;
  }

  if (buttonState2 == HIGH && isPressing) {
    releasedTime = millis()
    long totalPressDuration = releasedTime - pressedTime;
    isPressing = false;
  } if (totalPressDuration > DEBOUNCE_TIME && !hold) {
    // Button 2 Press function
  }
}


void button3()
{

  static bool isPressing = false;
  static bool hold = false;

  buttonState3 = digitalRead(buttonPin3);

  if (buttonState3 == HIGH && !isPressing) {
    pressedTime = millis();
    isPressing = true;
    hold = false;
  }
  if (buttonState3 == HIGH && isPressing && !hold) {
    long pressDuration = millis() - pressedTime;
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
    // Button 3 Press function
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

void blinkLed(int led_index int thisLed, int ledInterval) // 10 - LED light-up function, takes led number and desired flash interval
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

// Changing time function

