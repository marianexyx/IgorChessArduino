#include <LiquidCrystal.h>

//zmienne sklejające dane
bool bCoreFullDataBlock = false; //czy są jakieś dane do wysłania
char buffer;
String strDataReceived;

const int nLedFrom(16), nLedTo(17), nLedSend(18), nLedClear(19), nLedPlayer(20), nLedIgor(21);

const int BtnAPin(0), BtnBPin(1), BtnCPin(2), BtnDPin(3), BtnEPin(4), BtnFPin(5), BtnGPin(6), BtnHPin(7);
const int Btn1Pin(8), Btn2Pin(9), Btn3Pin(10), Btn4Pin(11), Btn5Pin(12), Btn6Pin(13), Btn7Pin(14), Btn8Pin(15);
bool bBtnAState(0), bBtnBState(0), bBtnCState(0), bBtnDState(0), bBtnEState(0), bBtnFState(0), bBtnGState(0), bBtnHState(0);
bool bBtn1State(0), bBtn2State(0), bBtn3State(0), bBtn4State(0), bBtn5State(0), bBtn6State(0), bBtn7State(0), bBtn8State(0);

const int startBtnPin(9), sendBtnPin(10), clearBtnPin(8);
bool startBtnState(0), sendBtnState(0), clearBtnState(0);
bool bStartOnce(0), bSendOnce(0), bClearOnce(0);

bool bGameJustStarted = false;
bool bEnemyTurn = false; //czy gracz mozę już typwoać swój ruch
bool bStartAvailable = false; //flaga sprawdzająca czy możemy wcisnąć przycisk START (po połączeniu z core'm)
bool bSendAvailable = false; //flaga sprawdzająca czy możemy wcisnąć przycisk WYSLIJ i wyslac dane na core
bool bClearAvailable = false; //flaga sprawdzająca czy jest co czyścić (czy był częściowo/całościowo wbity ruch gracza)
bool bConfirmedReset = false; //flaga po której sprawdzamy czy gracz potwierdził to że chce zresetować istniejącą grę
bool bPromoteAvailable = false; //flaga po ktorej sprawdzamy czy mamy do czynienia z promocja
String strPromoteType = 0;
String strPromotePiece = 0;

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

  if (bEnemyTurn == true //jeżeli gracz wykonuje ruch, to czytaj kolejne stany z przycisków i reaguj na to...
      && bPromoteAvailable = false) //...ale nie gdy mamy do czynienia z promocja

  {
    //funkcje wciskania przycisków są tak zrobione, że nie trzeba ich dodatkowo zabezpieczać
    if (strLetterFrom == "-1") strLetterFrom = lettersButtonsActivated('f'); //wprowadzenie litery pola skąd
    else if (strLetterFrom != "-1" && nDigitFrom == -1) nDigitFrom = digitsButtonsActivated('f'); //wprowadzenie cyfry pola skąd
    else if (nDigitFrom != -1 && strLetterFrom == "-1") strLetterTo = lettersButtonsActivated('t'); //wprowadzenie litery pola do
    else if (strLetterTo != "-1" && nDigitTo == -1) nDigitTo = digitsButtonsActivated('t');  //wprowadzenie cyfry pola do

    if (strLetterFrom != "-1" && nDigitFrom != -1 && strLetterTo != "-1" && nDigitTo != -1)
    {
      printLcd("Chcesz wykonac ruch z pola " + strLetterFrom + (String)nDigitFrom + " na pole " + strLetterTo + (String)nDigitTo
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
    else if (strDataReceived == "BAD_MOVE")
    {
      bEnemyTurn = true;
      printLcd("Nieprawidlowe zadanie ruchu. Wprowadz ruch ponownie");
    }
    else if (strDataReceived == "promote")
    {
      bEnemyTurn = true; //abysmy mogl dzialac w przycisk send i by sie dioda sciecila
      bPromoteAvailable = true;
      printLcd("Promocja piona. Przemien na: a-wieza, b-skoczek, c-goniec, d-hetman");
      ledEnemysMove(true, false); //prewencyjnie
    }
    else //fukcja serwisowa: echo back
    {
      String strDataToSend = "@echo: " + strDataReceived + "$";
      Serial.print(strDataToSend);
    }
    bCoreFullDataBlock = false; //wiadomość otrzymana. wyczyść flagę
    strDataReceived = "";
  }

  if (bPromoteAvailable == true)
  {
    if (strPromotePiece = 0) //zabezpieczenie przed mozliwascia kolejnego wcisniecia podczas potwierdzania promocji
      strPromoteType = promote(); //czekaj az gracz wcisnie przycisk odpowiadajacy promocji
    if (strPromoteType != 0)
    {
      if (strPromoteType == "a") strPromotePiece = "wieze";
      else if (strPromoteType == "b") strPromotePiece = "skoczka";
      else if (strPromoteType == "c") strPromotePiece = "gonca";
      else if (strPromoteType == "d") strPromotePiece = "hetmana";
      printLcd("Potwierdz promocje na " + strPromotePiece + " przyciskiem WYSLIJ");
      bSendAvailable = true; //reszta odbywa sie w przycisku send (wteyd gdy go wcisniemy)
    }
  }
}
//END OF LOOP



//FUNCTIONS
bool button(bool bBtnState, bool bWriteOnce, char chBtnType) //ogarniaj 3 przyciski funkcyjne
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
          bPromoteAvailable = false;
          strDataReceived = ""; //prewencyjnie
          strLetterFrom = "-1"; //prewencyjnie
          strLetterTo = "-1"; //prewencyjnie
          nDigitFrom = -1;  //prewencyjnie
          nDigitTo = -1; //prewencyjnie
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
        if (strPromotePiece == 0) //jezeli to nie jest promocja
        {
          printLcd("Wyslano ruch: " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo);
          Serial.print("@move " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo + "$"); //move e2e4
        }
        else //jezeli potwierdzamy promocje
        {
          printLcd("Pion zostal promowany na " + strPromotePiece);
          bPromoteAvailable = false; //koniec sytuacji promocji
          strPromotePiece = 0; //czyscimy by nie wpadac w warunki promocji
          Serial.print("promote_to " + strPromoteType);
          strPromoteType = 0; //czyszczenie by nie wpadac w warunek pytania o promocje
        }
        bEnemyTurn = false;
        bGameJustStarted = false; //prewencyjnie
        bClearAvailable = false; //wysłano ruch. nie ma zatem już nic do czyszczenia
        bSendAvailable = false; //ruch wysłano i nie można już wciskać przycisku WYSLIJ
        digitalWrite(nLedSend, LOW);
        ledEnemysMove(false, false); //zgaś obie diody, które pokazują czy teraz wbijany był ruch skąd, czy dokąd
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

  bBtnAState = digitalRead(BtnAPin); bBtnBState = digitalRead(BtnBPin);
  bBtnCState = digitalRead(BtnCPin); bBtnDState = digitalRead(BtnDPin);
  bBtnEState = digitalRead(BtnEPin); bBtnFState = digitalRead(BtnFPin);
  bBtnGState = digitalRead(BtnGPin); bBtnHState = digitalRead(BtnHPin);

  if (bBtnAState == 1) return "a";
  else if (bBtnBState == 1) return "b";
  else if (bBtnCState == 1) return "c";
  else if (bBtnDState == 1) return "d";
  else if (bBtnEState == 1) return "e";
  else if (bBtnFState == 1) return "f";
  else if (bBtnGState == 1) return "g";
  else if (bBtnHState == 1) return "h";
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

  bBtn1State = digitalRead(Btn1Pin); bBtn2State = digitalRead(Btn2Pin);
  bBtn3State = digitalRead(Btn3Pin); bBtn4State = digitalRead(Btn4Pin);
  bBtn5State = digitalRead(Btn5Pin); bBtn6State = digitalRead(Btn6Pin);
  bBtn7State = digitalRead(Btn7Pin); bBtn8State = digitalRead(Btn8Pin);

  if (bBtn1State == 1) return 1;
  else if (bBtn2State == 1) return 2;
  else if (bBtn3State == 1) return 3;
  else if (bBtn4State == 1) return 4;
  else if (bBtn5State == 1) return 5;
  else if (bBtn6State == 1) return 6;
  else if (bBtn7State == 1) return 7;
  else if (bBtn8State == 1) return 8;
  else
  {
    Serial.print("BtnStates value error");
    return 0;
  }
}

String promote()
{
  bBtnAState = digitalRead(BtnAPin); bBtnBState = digitalRead(BtnBPin);
  bBtnCState = digitalRead(BtnCPin); bBtnDState = digitalRead(BtnDPin);

  if (bBtnAState == 1) return "a";
  else if (bBtnBState == 1) return "b";
  else if (bBtnCState == 1) return "c";
  else if (bBtnDState == 1) return "d";
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
  digitalWrite(nLedTo, ledTo);
}

