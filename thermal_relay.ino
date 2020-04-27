// this example is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#include "max6675.h"
#include <LiquidCrystal.h>
// Thermocouple
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Relay
int relay = 3;

// ???
int vccPin = 3;
int gndPin = 2;

// LCD Display
int rs = 13;
int e = 12;
int d4 = 11;
int d5 = 10;
int d6 = 9;
int d7 = 8;
LiquidCrystal lcd = LiquidCrystal(rs, e, d4, d5, d6, d7);

// CTRL
int trgDec = 2;
int trgInc = 1;
int cycInc = 0;

long lastDebounceTime = 0;
long debounceDelay = 250;

int incSelect[] = {1,5,10,50,100};
int incIndex = 0;
float target = 300.0;
  
void setup() {
  Serial.begin(9600);
  // use Arduino pins 
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  pinMode(relay, OUTPUT); digitalWrite(relay, HIGH);

  // set columns and rows
  lcd.begin(16, 2);
   // Set the cursor on the third column and the first row, counting starts at 0:
  lcd.setCursor(2, 0);
  // Print the string 'Hello World!':
  lcd.print("Welcome to");
  // Set the cursor on the third column and the second row:
  lcd.setCursor(2, 1);
  // Print the string 'LCD tutorial':
  lcd.print("thermo-relay");

  // button inputs
  pinMode(trgDec, INPUT);
  pinMode(trgInc, INPUT);
  pinMode(cycInc, INPUT);
  
  // wait for MAX chip to stabilize
  delay(500);
  clearDisplay();
}

void clearDisplay() {
  lcd.setCursor(2, 0);
  // Print the string 'Hello World!':
  lcd.print("                ");
  // Set the cursor on the third column and the second row:
  lcd.setCursor(2, 1);
  // Print the string 'LCD tutorial':
  lcd.print("                ");
}

/* |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|
 * |T|M|P|:|#|#|#|#|F| |I|C|:|#|#|#|
 * |T|R|G|:|#|#|#|#|F| | | | | | | |
 */

void printTemp(float temp) {
   if (!isnan(temp)) {
     lcd.setCursor(0, 0);
     lcd.print("AC:     ");
     lcd.setCursor(3, 0);
     lcd.print(int(temp));
     lcd.setCursor(7, 0);
     lcd.print("F");
   }
}

void printTarget(float target) {
   lcd.setCursor(0, 1);
   lcd.print("TR:     ");
   lcd.setCursor(3, 1);
   lcd.print(int(target));
   lcd.setCursor(7, 1);
   lcd.print("F");
}

void printIncrement(int inc) {
   lcd.setCursor(10, 0);
   lcd.print("IC:     ");
   lcd.setCursor(13, 0);
   lcd.print(int(inc));
}

void loop() {
  // basic readout test, just print the current temp
   float f = thermocouple.readFahrenheit();

   int readTrgDec = analogRead(trgDec);
   int readTrgInc = analogRead(trgInc);
   int readCycInc = analogRead(cycInc);
   if ((millis() - lastDebounceTime) > debounceDelay) {
     if (readTrgDec > 1000) {
      target = target - incSelect[incIndex];
     }
     if (readTrgInc > 1000) {
      target = target + incSelect[incIndex];
     }
     if (readCycInc > 1000) {
      incIndex += 1;
      if (incIndex >= sizeof(incSelect) / sizeof(incSelect[0])) {
        incIndex = 0;
      }
     }
  
     if (isnan(f)) {
      // check for nan
     } else if (f > target) {
      digitalWrite(relay, LOW);
     } else {
      digitalWrite(relay, HIGH);
     }
     lastDebounceTime = millis();
   }
   // Serial.print("F:\t");
   // Serial.println(f);
   
   // Serial.print("trgDec:\t");
   // Serial.println(readTrgDec);
   
   // Serial.print("readTrgInc:\t");
   // Serial.println(readTrgInc);
   
   // Serial.print("readCycInc:\t");
   // Serial.println(readCycInc);
   
   printTemp(f);
   printTarget(target);
   printIncrement(incSelect[incIndex]);
   // delay(100);
}
