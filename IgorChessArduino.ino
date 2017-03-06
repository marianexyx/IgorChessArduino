#include <LiquidCrystal.h>

bool bCoreFullDataBlock;
char buffer;
String strDataToSend;

const int a1BtnPin(0), b2BtnPin(1), c3BtnPin(2), d4BtnPin(3); 
const int e5BtnPin(4), f6BtnPin(5), g7BtnPin(6), h8BtnPin(7);
const int clearButtonPin(8), startButtonPin(9), sendButtonPin(10);
const int nLedFrom(13), nLedTo(12);

int startButtonState = 0;
int sendButtonState = 0;
bool bWroteOnce1 = false;
bool bWroteOnce2 = false;

bool bGameJustStarted = false;
bool bEnemyTurn = false; //czy gracz mozę już typwoać swój ruch

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//todo: zamiast podciągać w dół rezystorami mogę podciągać w górę programowo...
//...

void setup()
{
  Serial.begin(9600);
  pinMode(nLedFrom, OUTPUT);
  pinMode(nLedTo, OUTPUT);
  bCoreFullDataBlock = false;

  pinMode(startButtonPin, INPUT);
  pinMode(sendButtonPin, INPUT);

  digitalWrite(nLedFrom, LOW);
  digitalWrite(nLedTo, LOW);

  lcd.begin(20, 4);
  lcd.print("Arduino is ready.");
  lcd.display(); // Turn on the display. TODO: czy to jest obowiazkowe
}

void loop()
{
  skladajPrzychodzaceDane();

  startButtonState = digitalRead(startButtonPin);
  sendButtonState = digitalRead(sendButtonPin);

  if (startButtonState && !bWroteOnce1)
  {
    if (bGameJustStarted == false)
    {
      Serial.write("@start$"); //println dodawałby '\r\n' co psuje '$'. chyba.
    }
    else 
    {
      Serial.write("@doFirstIgorMove$"); 
      bGameJustStarted = false;
    }

    bWroteOnce1 = true;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  else if (!startButtonState && bWroteOnce1)
  {
    bWroteOnce1 = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }

  if (sendButtonState && !bWroteOnce2)
  {
    Serial.write("@send$");
    //bGameJustStarted = false; //TODO:??

    bWroteOnce2 = true;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  else if (!sendButtonState && bWroteOnce2)
  {
    bWroteOnce2 = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }

  if (bCoreFullDataBlock == true)
  {
    if (strDataToSend == "connected")
    {
      lcd.clear();
      lcd.print("Connected to core.");
    }
    else if (strDataToSend == "started")
    {
      bGameJustStarted = true;
      lcd.clear();
      lcd.print("Wcisnij start, aby rozpaczac gre.");
    }
    else if (strDataToSend == "IgorHasEndedMove")
    {
      lcd.clear();
      lcd.print("Wykonaj swoj ruch.");
      ReadyForEnemysMove();
    }
    else //echo back
    {
      strDataToSend = "@echo: " + strDataToSend + "$";
      Serial.print(strDataToSend);
      bCoreFullDataBlock = false;
      //strDataToSend = "";
    }
    strDataToSend = "";
  }
}

void skladajPrzychodzaceDane()
{
  while (Serial.available() > 0)
  {
    buffer = Serial.read();
    if (buffer == '$') {
      bCoreFullDataBlock = true;
      break;
    }
    strDataToSend += buffer;
  }
}

void ReadyForEnemysMove()
{
   bEnemyTurn = true;
   //sygnał dla gracza że czekamy na wybór bierki do ruszenia
   digitalWrite(nLedFrom, HIGH); 
}

