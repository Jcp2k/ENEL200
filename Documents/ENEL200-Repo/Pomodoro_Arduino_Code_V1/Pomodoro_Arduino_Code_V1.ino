
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
  WORK_1,  // LED 0 solid
  BREAK_1, // LED 0 blinking
  WORK_2,  // LED 1 solid
  BREAK_2, // LED 1 blinking
  WORK_3,  // LED 2 solid
  BREAK_3, // LED 2 blinking
  WORK_4,  // LED 3 solid
  BREAK_4, // LED 3 blinking
  IDLE     // All LEDs off
};


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
  pinMode(buzPin, OUPUT);

// Initialising PomodoroCycle Function
startPomoPeriodMillis = millis();

}

void loop() {
  // Reading states of buttons
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);    
}
<<<<<<< HEAD
// LED function - Done [Q]
// Pomodoro Cycle
=======

>>>>>>> 63d298b7f57e45a9052b94c58ba24f5f4d7d7df7
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


void onTimerStartUp()
{
  unsigned long workPeriod = 25 * millisecondsToMinutes;
  unsigned long shortBreakPeriod = 5 * millisecondsToMinutes;
  unsigned long LongBreakPeriod = 15 * millisecondsToMinutes;
  pomodoroCycle(workPeriod, shortBreakPeriod, LongBreakPeriod, \isButton2paused\);
}

void pomodoroCycle(workPeriod, shortBreakPeriod, LongBreakPeriod, \isButton2paused\) // When button two is on, and timer should be running.
{
  // Set cycleNumber to work1
  currentPomoPeriodMillis = millis();
  if (currentPomoPeriodMillis - startPomoPeriodMillis >= workPeriod) {
    startPomoPeriodMillis = currentPomoPeriodMillis;
    // Set cycleNumber to break1
    
}

}

void DisplayAndMultiplex()
//Turn off both MOSFETs (blank)
//Shift in digit 1's 7-bit segment pattern
//Turn on digit 1's MOSFET, hold ~1-5 ms
//Turn off digit 1's MOSFET
//Shift in digit 2's 7-bit segment pattern
//Turn on digit 2's MOSFET, hold ~1-5 ms
//Loop





<<<<<<< HEAD
void blinkLed(int thisLed, int ledInterval) // 10 - LED light-up function, takes led number and desired flash interval
=======
// LED function - Done [Q]

void turnOnLED(int thisLed)
{
  digitalWrite(thisLed, HIGH);
}

void turnOffLED(int thisLed) {
  digitalWrite(thisLed, LOW);
}

void blinkLed(int led_index int thisLed, int ledInterval) // 10 - LED light-up function, takes led number and desired flash interval
>>>>>>> 63d298b7f57e45a9052b94c58ba24f5f4d7d7df7
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

