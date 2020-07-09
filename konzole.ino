#include <Arduino.h>
#include <SevSeg.h>

#define RED 2
#define YELLOW 3
#define GREEN 4

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
  int displayType =
      COMMON_CATHODE;  // Your display is either common cathode or common anode
                       // This pinout is for a bubble dispaly
  // Declare what pins are connected to the GND pins (cathodes)
  int digit1 = 48;  // Pin 1
  int digit2 = 49;  // Pin 10
  int digit3 = 50;  // Pin 4
  int digit4 = 51;  // Pin 6

  // Declare what pins are connected to the segments (anodes)
  int segA = 32;   // Pin 12
  int segB = 33;   // Pin 11
  int segC = 34;   // Pin 3
  int segD = 35;   // Pin 8
  int segE = 36;   // Pin 2
  int segF = 37;   // Pin 9
  int segG = 38;   // Pin 7
  int segDP = 39;  // Pin 5

  int numberOfDigits = 4;  // Do you have a 1, 2 or 4 digit display?

  sevseg.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4,
               segA, segB, segC, segD, segE, segF, segG, segDP);

  sevseg.SetBrightness(100);  // Set the display to 100% brightness level

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
  Serial.begin(9600);
  timerBomb = millis();
  BUM = millis() + TIMER;
}

void loop() {
  // remain time
  printTime();
  // change color
  int color = analogRead(POT);
  color = 1023 - color;  // potak se toci obracene
  color = map(color, 0, 1023, 2, 5);
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
        sevseg.DisplayString("SAUE", 0);
        while (1) sevseg.DisplayString("SAUE", 0);
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
        delay(800);
        printTime();
        off();
        delay(200);
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
  char tempString[5];
  unsigned long remain = BUM - millis();
  int min, sec;
  remain /= 1000;
  min = remain / 60;
  sec = remain % 60;
  sprintf(tempString, "%04d", sec + min * 100);
  sevseg.DisplayString(tempString, 2);
  if (remain == 0) {
    digitalWrite(WARN, HIGH);
    sevseg.DisplayString("buch", -1);
    while (true) sevseg.DisplayString("buch", -1);
  }
}