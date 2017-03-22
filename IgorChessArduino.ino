#include <Wire.h>
#include <Keypad.h>
#include "lcd.h"
#include "vars.h"
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
  skladajPrzychodzaceDane();
  ledWhoseTurn();
  gameStatus();

  /*//cały czas odczytuj stan przycisków
    customKey = customKeypad.getKey(); //odczytywany wciskany przycisk
    startBtnState = digitalRead(startBtnPin);
    sendBtnState = digitalRead(sendBtnPin);
    clearBtnState = digitalRead(clearBtnPin);

    //TODO: blokada by nie dało się wcisnąć przycisków ABC...123... i funcykjnych jaks SEND jednocześnie

    //funkcje ktore wyslą wiadomość raz, mimo iż ich przyciski są ciągle wciśnięte
    bBtnStartOnce = button(startBtnState, bStartOnce, 't');
    bBtnSendOnce = button(sendBtnState, bSendOnce, 'd');
    bBtnClearOnce = button(clearBtnState, bClearOnce, 'c');*/

  /*if (bEnemyTurn == true //jeżeli gracz wykonuje ruch, to czytaj kolejne stany z przycisków i reaguj na to...
      && bPromoteAvailable == false) //...ale nie gdy mamy do czynienia z promocja
    {
    //funkcje wciskania przycisków są tak zrobione, że nie trzeba ich dodatkowo zabezpieczać
    if (strGameStatus == "selectLetterFrom") strLetterFrom = lettersButtonsActivated('f'); //wprowadzenie litery pola skąd
    else if (strGameStatus == "selectDigitFrom") nDigitFrom = digitsButtonsActivated('f'); //wprowadzenie cyfry pola skąd
    else if (strGameStatus == "selectLetterTo") strLetterTo = lettersButtonsActivated('t'); //wprowadzenie litery pola do
    else if (strGameStatus == "selectDigitTo") nDigitTo = digitsButtonsActivated('t');  //wprowadzenie cyfry pola do

    if (strGameStatus == "moveIsReady")
    {
      printLcd(CONFIRM_MOVE, strLetterFrom + (String)nDigitFrom, strLetterTo + (String)nDigitTo);
      bSendAvailable = true; //możemy już wciskać przycisk "send"
      digitalWrite(nLedSend, HIGH);
      strGameStatus = "moveWaitsToBeSend";
    }
    }*/

  if (bCoreFullDataBlock == true) //odczytywanie danych po serialu z core'a
  {
    if (strDataReceived == "started") nGameStatus = STARTED;
    else if (strDataReceived == "IgorHasEndedMove" || strDataReceived == "PromotedTo") nGameStatus = SELECT_LETTER_FROM;
    else if (strDataReceived.substring(0, 8) == "BAD_MOVE") nGameStatus = BAD_MOVE;
    else if (strDataReceived == "promote") nGameStatus = PROMOTE;
    else if (strDataReceived.substring(0, 8) == "GameOver")
    {
      String strWhoWon = strDataReceived.substring(9);
      if (strWhoWon == "white_won") nGameStatus = WHITE_WON;
      else if (strWhoWon == "black_won") nGameStatus = BLACK_WON;
      else if (strWhoWon == "draw") nGameStatus = DRAW;
      else Serial.print("ERROR: wrong strWhoWon parameter");
    }
    else //fukcja serwisowa: echo back
    {
      String strDataToSend = "@echo: " + strDataReceived + "$";
      Serial.print(strDataToSend);
    }
    bCoreFullDataBlock = false; //wiadomość otrzymana. wyczyść flagę
    strDataReceived = "";
  }

  /*if (bPromoteAvailable == true) //TODO: pod nowe przyciski to ogarnąć
    {
    if (strPromotePiece = "0") //zabezpieczenie przed mozliwascia kolejnego wcisniecia podczas potwierdzania promocji
      strPromoteType = promote(); //czekaj az gracz wcisnie przycisk odpowiadajacy promocji
    if (strPromoteType != "0")
    {
      if (strPromoteType == "a") strPromotePiece = "wieze";
      else if (strPromoteType == "b") strPromotePiece = "skoczka";
      else if (strPromoteType == "c") strPromotePiece = "gonca";
      else if (strPromoteType == "d") strPromotePiece = "hetmana";
      printLcd(CONFIRM_PROMOTION, strPromotePiece);
      bSendAvailable = true; //reszta odbywa sie w przycisku send (wteyd gdy go wcisniemy)
    }
    }*/
}

