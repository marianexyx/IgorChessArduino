#include <Wire.h>
#include <Keypad.h>
#include "vars.h"
#include "lcd.h"
#include "functions.h"

void setup()
{
  Serial.begin(9600);
  pinMode(nLedFrom, OUTPUT); pinMode(nLedTo, OUTPUT); pinMode(nLedSend, OUTPUT); pinMode(nLedClear, OUTPUT);
  pinMode(nLedPlayer, OUTPUT); pinMode(nLedIgor, OUTPUT);
  pinMode(nLedTest, OUTPUT);

  pinMode(startBtnPin, INPUT); pinMode(sendBtnPin, INPUT); pinMode(clearBtnPin, INPUT);

  digitalWrite(nLedFrom, LOW); digitalWrite(nLedTo, LOW); digitalWrite(nLedSend, LOW); digitalWrite(nLedClear, LOW);
  digitalWrite(nLedPlayer, LOW); digitalWrite(nLedIgor, LOW);
  digitalWrite(nLedTest, LOW);

  lcd.init();
  lcd.backlight();
  printLcd(ARDUINO_READY);
}

void loop()
{
  gameStatus();
  skladajPrzychodzaceDane();
  sprawdzajPrzychodzaceDane();
  //ledWhoseTurn();
}

