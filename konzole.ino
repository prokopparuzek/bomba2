#include <Arduino.h>
#include <SevSeg.h>

#define RED 4
#define YELLOW 3
#define GREEN 2

#define BUT 10

#define WARN 13

#define POT A0

#define TIMER 120 * 1000L

// bomba
byte simon[10] = {RED,   RED,   GREEN, YELLOW, YELLOW,
                  GREEN, GREEN, RED,   GREEN,  RED};
byte index = 0;
unsigned long int timerBomb = 0;
unsigned long int BUM;

// display init
SevSeg sevseg;  // Instantiate a seven segment object

void setup() {
  // display setup
  byte numDigits = 4;
  byte digitPins[] = {48, 49, 50, 51};
  byte segmentPins[] = {32, 33, 34, 35, 36, 37, 38, 39};
  bool resistorsOnSegments =
      false;  // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE;  // See README.md for options
  bool updateWithDelays = false;       // Default 'false' is Recommended
  bool leadingZeros =
      true;  // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint =
      false;  // Use 'true' if your decimal point doesn't exist or isn't
              // connected. Then, you only need to specify 7 segmentPins[]

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins,
               resistorsOnSegments, updateWithDelays, leadingZeros,
               disableDecPoint);
  // LED setup
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  // button setup
  pinMode(BUT, INPUT_PULLUP);
  // warn setup
  pinMode(WARN, OUTPUT);
  // pot setup
  pinMode(POT, INPUT);
  // Serial setup
  // Serial.begin(9600);
  timerBomb = millis();
  BUM = millis() + TIMER;
  sevseg.setNumber(1234);
}

void loop() {
  sevseg.refreshDisplay();
  // remain time
  printTime();
  // change color
  int color = analogRead(POT);
  color = map(color, 0, 1023, 9, 11);
  switch (color) {
    case GREEN:
      green();
      break;
    case YELLOW:
      yellow();
      break;
    case RED:
      red();
      break;
  }
  // add color
  if (digitalRead(BUT) == LOW) {
    if (color == simon[index]) {
      index++;
      // defuse
      if (index == 10) {
        sevseg.setChars("SAVE");
        while (1) sevseg.refreshDisplay();
      }
      for (int i = 0; i < index; i++) {
        switch (simon[i]) {
          case GREEN:
            green();
            break;
          case YELLOW:
            yellow();
            break;
          case RED:
            red();
            break;
        }
        delay(1000);
        printTime();
        off();
        noTone(WARN);
        delay(250);
        printTime();
      }
    } else {
      digitalWrite(WARN, HIGH);
      index = 0;
      delay(1000);
      digitalWrite(WARN, LOW);
    }
  }
}

void red() {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
}

void yellow() {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
}

void green() {
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}

void off() {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
}

void printTime() {
  unsigned long remain = BUM - millis();
  int min, sec;
  remain /= 1000;
  min = remain / 60;
  sec = remain % 60;
  sevseg.setNumber(sec + min * 100, 2);
  if (remain == 0) {
    sevseg.setChars("BUM");
    digitalWrite(WARN, HIGH);
    while (true) sevseg.refreshDisplay();
  }
}