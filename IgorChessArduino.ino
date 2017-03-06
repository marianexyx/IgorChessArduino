#include <LiquidCrystal.h>

//zmienne sklejające dane
bool bCoreFullDataBlock = false; //czy są jakieś dane do wysłania
char buffer;
String strDataReceived;

const int nLedFrom(16), nLedTo(17), nLedSend(18), nLedClear(19), nLedPlayer(20), nLedIgor(21);

const int BtnAPin(0), BtnBPin(1), BtnCPin(2), BtnDPin(3), BtnEPin(4), BtnFPin(5), BtnGPin(6), BtnHPin(7);
const int Btn1Pin(8), Btn2Pin(9), Btn3Pin(10), Btn4Pin(11), Btn5Pin(12), Btn6Pin(13), Btn7Pin(14), Btn8Pin(15);

const int startBtnPin(9), sendBtnPin(10), clearBtnPin(8);
bool startBtnState(0), sendBtnState(0), clearBtnState(0);
bool bStartOnce(0), bSendOnce(0), bClearOnce(0);

bool bGameJustStarted = false;
bool bEnemyTurn = false; //czy gracz mozę już typwoać swój ruch
bool bStartAvailable = false; //flaga sprawdzająca czy możemy wcisnąć przycisk START (po połączeniu z core'm)
bool bSendAvailable = false; //flaga sprawdzająca czy możemy wcisnąć przycisk WYSLIJ i wyslac dane na core
bool bClearAvailable = false; //flaga sprawdzająca czy jest co czyścić (czy był częściowo/całościowo wbity ruch gracza)
bool bConfirmedReset = false; //flaga po której sprawdzamy czy gracz potwierdził to że chce zresetować istniejącą grę

String strLetterFrom("-1"), strLetterTo("-1");
int nDigitFrom(-1), nDigitTo(-1);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//todo: zamiast podciągać w dół rezystorami mogę podciągać w górę programowo...
//...

void setup()
{
  Serial.begin(9600);
  pinMode(nLedFrom, OUTPUT); pinMode(nLedTo, OUTPUT); pinMode(nLedSend, OUTPUT); pinMode(nLedClear, OUTPUT);
  pinMode(nLedPlayer, OUTPUT); pinMode(nLedIgor, OUTPUT);

  pinMode(startBtnPin, INPUT); pinMode(sendBtnPin, INPUT); pinMode(clearBtnPin, INPUT);
  pinMode(BtnAPin, INPUT); pinMode(BtnBPin, INPUT); pinMode(BtnCPin, INPUT); pinMode(BtnDPin, INPUT);
  pinMode(BtnEPin, INPUT); pinMode(BtnFPin, INPUT); pinMode(BtnGPin, INPUT); pinMode(BtnHPin, INPUT);
  pinMode(Btn1Pin, INPUT); pinMode(Btn2Pin, INPUT); pinMode(Btn3Pin, INPUT); pinMode(Btn4Pin, INPUT);
  pinMode(Btn5Pin, INPUT); pinMode(Btn6Pin, INPUT); pinMode(Btn7Pin, INPUT); pinMode(Btn8Pin, INPUT);

  digitalWrite(nLedFrom, LOW); digitalWrite(nLedTo, LOW); digitalWrite(nLedSend, LOW); digitalWrite(nLedClear, LOW);
  digitalWrite(nLedPlayer, LOW); digitalWrite(nLedIgor, LOW);

  lcd.begin(20, 4);
  lcd.print("Arduino is ready.");
  lcd.display(); // Turn on the display.
}

void loop()
{
  skladajPrzychodzaceDane();

  ledWhoseTurn();

  //cały czas odczytuj stan przycisków
  startBtnState = digitalRead(startBtnPin);
  sendBtnState = digitalRead(sendBtnPin); 
  clearBtnState = digitalRead(clearBtnPin);

  //funkcje ktore wyslą wiadomość raz, mimo iż ich przyciski są ciągle wciśnięte
  if (bStartAvailable == true) //przycisk start da się wcisnąć tylko gdy mamy połączenie z core
    bStartOnce = button(startBtnState, bStartOnce, 't');
  bSendOnce = button(sendBtnState, bSendOnce, 'd'); 
  bClearOnce = button(clearBtnState, bClearOnce, 'c');

  if (bEnemyTurn == true) //jeżeli gracz wykonuje ruch, to czytaj kolejne stany z przycisków i reaguj na to
  {
    //funkcje wciskania przycisków są tak zrobione, że nie trzeba ich dodatkowo zabezpieczać
    if (strLetterFrom == "-1") strLetterFrom = lettersButtonsActivated('f'); //wprowadzenie litery pola skąd
    else if (strLetterFrom != "-1" && nDigitFrom == -1) nDigitFrom = digitsButtonsActivated('f'); //wprowadzenie cyfry pola skąd
    else if (nDigitFrom != -1 && strLetterFrom == "-1") strLetterTo = lettersButtonsActivated('t'); //wprowadzenie litery pola do
    else if (strLetterTo != "-1" && nDigitTo == -1) nDigitTo = digitsButtonsActivated('t');  //wprowadzenie cyfry pola do

    if (strLetterFrom != "-1" && nDigitFrom != -1 && strLetterTo != "-1" && nDigitTo != -1)
    {
      printLcd("Chcesz wykonac ruch z pola " + strLetterFrom + (String)nDigitFrom + " na pole " + strLetterTo + (String)nDigitTo)
      + ". Potwierdz wciskajac WYSLIJ, lub popraw wciskajac WYCZYSC");
      bSendAvailable = true; //możemy już wciskać przycisk "send"
      digitalWrite(nLedSend, HIGH);
    }
  }

  if (bCoreFullDataBlock == true)
  {
    if (strDataReceived == "connected")
    {
      bGameJustStarted = false; //prewencyjnie
      bStartAvailable = true; //da się wcisnąć start, by móc rozpocząć grę w systemie
      bEnemyTurn = true;
      printLcd("Connected to core");
    }
    else if (strDataReceived == "started")
    {
      bGameJustStarted = true; //nowa gra ruszyła w pamieci. AI czeka z ruchem. wciśnięcie START rozpocznie ruch
      bStartAvailable = true; //prewencyjnie
      bEnemyTurn = true;
      printLcd("Wcisnij start, aby rozpaczac gre");
    }
    else if (strDataReceived == "IgorHasEndedMove")
    {
      bEnemyTurn = true;
      printLcd("Wykonaj swoj ruch");
    }
    else //fukcja serwisowa: echo back
    {
      String strDataToSend = "@echo: " + strDataReceived + "$";
      Serial.print(strDataToSend);
    }
    bCoreFullDataBlock = false; //wiadomość otrzymana. wyczyść flagę
    strDataReceived = "";
  }
}
//END OF LOOP



//FUNCTIONS
bool button(bool bBtnState, bool bWriteOnce, char chBtnType = 0)
{
  if (bEnemyTurn == true) //jeżeli jest tura gracza
  {
    if (bBtnState && !bWriteOnce) //wciskany przycisk uruchomi funkcję raz mimo długiego trzymania
    {
      if (chBtnType = 't') //start button
      {
        //jeżeli nie jesteśmy w fazie startu gry (rozgrywka trwa) i gracz chce zresetować rozgrywkę
        if (bGameJustStarted == false && bConfirmedReset == false) 
        {
          printLcd("Czy chcesz zresetować grę?");
          bConfirmedReset = true;
        }
        //jeżeli nie jesteśmy w fazie startu gry (rozgrywka trwa) i gracz potwierdził chęć zresetowania rozgrywki
        else if (bGameJustStarted == false && bConfirmedReset == true)
        {
          //czyszczenie
          bEnemyTurn = false;
          bConfirmedReset = false;
          bSendAvailable = false; //prewencyjnie
          bClearAvailable = false; //prewencyjnie
          bCoreFullDataBlock = false; //prewencyjnie
          strDataReceived = ""; //prewencyjnie
          strLetterFrom = "-1"; //prewencyjnie
          strLetterTo = "-1"; //prewencyjnie
          nDigitFrom = -1;  //prewencyjnie
          nDigitTo= -1;  //prewencyjnie
          digitalWrite(nLedFrom, LOW); //prewencyjnie
          digitalWrite(nLedTo, LOW); //prewencyjnie
          digitalWrite(nLedSend, LOW); //prewencyjnie
          digitalWrite(nLedClear, LOW); //prewencyjnie
          ledEnemysMove(false, false); //zgaś obie diody, które pokazują czy teraz wbijany był ruch skąd, czy dokąd
          
          Serial.print("@start$"); //to rozpocznij nową grę
        }
        else if (bGameJustStarted == true) //gra w pamięci rozpoczęta. przycisk start spowoduje pierwszy ruch białego
        {
          bGameJustStarted = false; //stan gry będzie jako rozpoczęty dopiero jak wykona się jakikolwiek ruch
          bEnemyTurn = false;
          Serial.print("@doFirstIgorMove$"); //gra ruszyła, niech AI wykona swój pierwszy ruch
        }
      }
      else if (chBtnType == 'd' && bSendAvailable == true) //send można wysłać tylko wtedy, gdy jest gotowy ruch do wysłania
      {
        printLcd("Wyslano ruch: " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo);
        bEnemyTurn = false;
        bGameJustStarted = false; //prewencyjnie
        bClearAvailable = false; //wysłano ruch. nie ma zatem już nic do czyszczenia
        bSendAvailable = false; //ruch wysłano i nie można już wciskać przycisku WYSLIJ
        digitalWrite(nLedSend, LOW);
        ledEnemysMove(false, false); //zgaś obie diody, które pokazują czy teraz wbijany był ruch skąd, czy dokąd
        Serial.print("@move " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo + "$"); //move e2e4
      }
      else if (chBtnType == 'c' && bClearAvailable == true) //czyszczenie ruchu, jeżeli cokolwiek było już wbijane
      {
        strLetterFrom = "-1";
        strLetterTo = "-1";
        nDigitFrom = -1;
        nDigitTo = -1;
        bClearAvailable = false; //wyczyszczone. nie da się od razu ponownie wcisnąć przycisku WYCZYSC
        digitalWrite(nLedClear, LOW);
        ledEnemysMove(false, false); //zgaś obie diody, które pokazują czy teraz wbijany był ruch skąd, czy dokąd
      }

      bWriteOnce = true; //cokolwiek wciśnięto, to raz to zrobiono i wykonano, mimo iż przycisk przez jakąś chwilę będzie wciśniety
      delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
    }
    else if (!bBtnState && bWriteOnce)
    {
      bWriteOnce = false;
      delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
    }
  }
  //jeżeli żadna zmiana nie zaszła, to zwrócone zostanie to samo co weszło, czyli nic się nie stanie
  return bWriteOnce;
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
    strDataReceived += buffer;
  }
}

void printLcd(String strLcdMsg)
{
  lcd.clear();
  lcd.print(strLcdMsg);
}

String lettersButtonsActivated(char chTargetField)
{
  if (chTargetField == 'f')
  {
    ledEnemysMove(true, false);
    printLcd("Wybierz bierke ktora chcesz wykonac ruch (przyciski A-H)");
    //jeszcze nic gracz nie wpisał, więc przycisk WYCZYSC jest ciagle nieaktywny
  }
  else if (chTargetField == 't')
  {
    bClearAvailable = true; //jest co czyścić
    digitalWrite(nLedClear, HIGH); //dioda mówi nam o tym że można poprawić żądanie ruchu
    ledEnemysMove(false, true);
    printLcd("Wybierz pole na ktore chcesz przemiescic bierke (przyciski A-H)");
  }
  else Serial.print("chTargetField value error");

  BtnAState = digitalRead(BtnAPin); BtnBState = digitalRead(BtnBPin);
  BtnCState = digitalRead(BtnCPin); BtnDState = digitalRead(BtnDPin);
  BtnEState = digitalRead(BtnEPin); BtnFState = digitalRead(BtnFPin);
  BtnGState = digitalRead(BtnGPin); BtnHState = digitalRead(BtnHPin);

  if (BtnAState == 1) return "a";
  else if (BtnBState == 1) return "b";
  else if (BtnCState == 1) return "c";
  else if (BtnDState == 1) return "d";
  else if (BtnEState == 1) return "e";
  else if (BtnFState == 1) return "f";
  else if (BtnGState == 1) return "g";
  else if (BtnHState == 1) return "h";
  else
  {
    Serial.print("BtnStates value error");
    return "0";
  }
}

int digitsButtonsActivated(char chTargetField)
{
  bClearAvailable = true; //jest co czyścić

  if (chTargetField == 'f')
    printLcd("Wybierz bierke ktora chcesz wykonac ruch (przyciski 1-8)");
  else if (chTargetField == 't')
    printLcd("Wybierz pole na ktore chcesz przemiescic bierke (przyciski 1-8)");
  else Serial.print("chTargetField value error");

  Btn1State = digitalRead(Btn1Pin); Btn2State = digitalRead(Btn2Pin);
  Btn3State = digitalRead(Btn3Pin); Btn4State = digitalRead(Btn4Pin);
  Btn5State = digitalRead(Btn5Pin); Btn6State = digitalRead(Btn6Pin);
  Btn7State = digitalRead(Btn7Pin); Btn8State = digitalRead(Btn8Pin);

  if (Btn1State == 1) return 1;
  else if (Btn2State == 1) return 2;
  else if (Btn3State == 1) return 3;
  else if (Btn4State == 1) return 4;
  else if (Btn5State == 1) return 5;
  else if (Btn6State == 1) return 6;
  else if (Btn7State == 1) return 7;
  else if (Btn8State == 1) return 8;
  else
  {
    Serial.print("BtnStates value error");
    return 0;
  }
}

void ledWhoseTurn() //led informujący gracza o tym czy może się ruszać (TODO: zamienić na "czekaj..." or smtg?)
{
    if (bEnemyTurn)
  {
    digitalWrite(nLedPlayer, HIGH);
    digitalWrite(nLedIgor, LOW);
  }
  else
  {
    digitalWrite(nLedPlayer, LOW);
    digitalWrite(nLedIgor, HIGH);
  }
}

void ledEnemysMove(bool ledFrom, bool ledTo) //światełka informujące o tym czy wbijamy teraz ruch skąd, czy dokąd
//TODO: może kolorowe czerwone wyświetlacze alfanumeryczne?
{
  digitalWrite(nLedFrom, ledFrom);
  digitalWrite(nLedFTo, ledTo);
}

