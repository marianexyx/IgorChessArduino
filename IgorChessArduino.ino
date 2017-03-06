#include <LiquidCrystal.h>

bool bCoreFullDataBlock;
char buffer;
String strDataToSend;

const int buttonPin1 = 8;
const int buttonPin2 = 9;
int buttonState1 = 0;
int buttonState2 = 0;
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
  pinMode(13, OUTPUT);
  bCoreFullDataBlock = false;

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);

  digitalWrite(13, LOW);

  lcd.begin(20, 4);
  lcd.print("###$$@$$$$@$$@$@$$");
  lcd.display(); // Turn on the display. TODO: czy to jest obowiazkowe
}

void loop()
{
  skladajPrzychodzaceDane();

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  if (buttonState1 && !bWroteOnce1)
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
  else if (!buttonState1 && bWroteOnce1)
  {
    bWroteOnce1 = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }

  if (buttonState2 && !bWroteOnce2)
  {
    Serial.write("@send$");
    //bGameJustStarted = false;

    bWroteOnce2 = true;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  else if (!buttonState2 && bWroteOnce2)
  {
    bWroteOnce2 = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }

  if (bCoreFullDataBlock == true)
  {
    if (strDataToSend == "started")
    {
      bGameJustStarted = true;
    }
    else if (strDataToSend == "enemyTurn")
    {
      bEnemyTurn = true;
      //TODO: niech coś się stanie
    }
    else //echo back
    {
      strDataToSend = "@echo: " + strDataToSend + "$";
      Serial.print(strDataToSend);
      digitalWrite(13, HIGH); //zaświeć diodą
      delay(2000);
      digitalWrite(13, LOW);
      bCoreFullDataBlock = false;
      strDataToSend = "";
    }
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

