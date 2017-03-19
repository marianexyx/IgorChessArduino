#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//zmienne sklejające dane
bool bCoreFullDataBlock = false; //czy są jakieś dane do wysłania
char buffer;
String strDataReceived;

const int nLedTest(10);
const int nLedFrom(1), nLedTo(1), nLedSend(1), nLedClear(1), nLedPlayer(1), nLedIgor(1);

const int startBtnPin(11), sendBtnPin(12), clearBtnPin(13);
bool startBtnState(0), sendBtnState(0), clearBtnState(0);
bool bStartOnce(0), bSendOnce(0), bClearOnce(0);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char hexaKeys[ROWS][COLS] = //define the cymbols on the buttons of the keypads
{
  {'a', 'b', 'c', 'd'},
  {'e', 'f', 'g', 'h'},
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};

byte rowPins  [ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins [COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey; //odczytywany wciskany przycisk

bool bGameJustStarted = false;
bool bEnemyTurn = false; //czy gracz mozę już typwoać swój ruch
bool bStartAvailable = false; //flaga sprawdzająca czy możemy wcisnąć przycisk START (po połączeniu z core'm)
bool bSendAvailable = false; //flaga sprawdzająca czy możemy wcisnąć przycisk WYSLIJ i wyslac dane na core
bool bClearAvailable = false; //flaga sprawdzająca czy jest co czyścić (czy był częściowo/całościowo wbity ruch gracza)
bool bConfirmedReset = false; //flaga po której sprawdzamy czy gracz potwierdził to że chce zresetować istniejącą grę
bool bPromoteAvailable = false; //flaga po ktorej sprawdzamy czy mamy do czynienia z promocja
bool bBadMove = false; //flaga po ktorej sprawdzamy czy gracz nie wyslal dopiero co błędnego zapytania o swój ruch
String strPromoteType = "0";
String strPromotePiece = "0";

String strLetterFrom("-1"), strLetterTo("-1");
int nDigitFrom(-1), nDigitTo(-1);

String strGameStatus = "";

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27. lcd display: 20 chars, 4 lines
bool bPrintOnce = false;

//todo: zamiast podciągać w dół rezystorami mogę podciągać w górę programowo...

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
  lcd.print("Arduino is ready.");
}

void loop()
{
  skladajPrzychodzaceDane();

  ledWhoseTurn();

  //cały czas odczytuj stan przycisków
  customKey = customKeypad.getKey(); //odczytywany wciskany przycisk
  startBtnState = digitalRead(startBtnPin);
  sendBtnState = digitalRead(sendBtnPin);
  clearBtnState = digitalRead(clearBtnPin);

  //TODO: blokada by nie dało się wcisnąć przycisków ABC...123... i funcykjnych jaks SEND jednocześnie

  //funkcje ktore wyslą wiadomość raz, mimo iż ich przyciski są ciągle wciśnięte
  if (bStartAvailable == true) //przycisk start da się wcisnąć tylko gdy mamy połączenie z core
    bStartOnce = button(startBtnState, bStartOnce, 't');
  bSendOnce = button(sendBtnState, bSendOnce, 'd');
  bClearOnce = button(clearBtnState, bClearOnce, 'c');

  if (bEnemyTurn == true //jeżeli gracz wykonuje ruch, to czytaj kolejne stany z przycisków i reaguj na to...
      && bPromoteAvailable == false) //...ale nie gdy mamy do czynienia z promocja
  {
    //funkcje wciskania przycisków są tak zrobione, że nie trzeba ich dodatkowo zabezpieczać
    if (strGameStatus == "selectLetterFrom") strLetterFrom = lettersButtonsActivated('f'); //wprowadzenie litery pola skąd
    else if (strGameStatus == "selectDigitFrom") nDigitFrom = digitsButtonsActivated('f'); //wprowadzenie cyfry pola skąd
    else if (strGameStatus == "selectLetterTo") strLetterTo = lettersButtonsActivated('t'); //wprowadzenie litery pola do
    else if (strGameStatus == "selectDigitTo") nDigitTo = digitsButtonsActivated('t');  //wprowadzenie cyfry pola do

    if (strGameStatus == "moveIsReady")
    {
      //printLcd("12345678901234567890");
      printLcd("Chcesz wykonac ruch",
               "z pola " + strLetterFrom + (String)nDigitFrom + " na " + strLetterTo + (String)nDigitTo + ".",
               //"z pola e2 na e4."
               "Potwierdz: WYSLIJ,",
               "lub popraw: WYCZYSC");
      bSendAvailable = true; //możemy już wciskać przycisk "send"
      digitalWrite(nLedSend, HIGH);
      strGameStatus = "moveWaitsToBeSend";
    }
  }

  if (bCoreFullDataBlock == true) //odczytywanie danych po serialu z core'a
  {
    if (strDataReceived == "started")
    {
      strGameStatus = strDataReceived;
      bGameJustStarted = true; //nowa gra ruszyła w pamieci. AI czeka z ruchem. wciśnięcie START rozpocznie ruch
      bStartAvailable = true; 
      bEnemyTurn = true;
      //printLcd("12345678901234567890");
      printLcd("Wcisnij start, aby",
               "rozpaczac gre");
    }
    else if (strDataReceived == "IgorHasEndedMove")
    {
      strGameStatus = "selectLetterFrom";
      bEnemyTurn = true;
    }
    else if (strDataReceived.substring(0, 8) == "BAD_MOVE")
    {
      bEnemyTurn = true;
      bBadMove = true;
      clearMove();
    }
    else if (strDataReceived == "promote")
    {
      bEnemyTurn = true; //abysmy mogl dzialac w przycisk send i by sie dioda sciecila
      bPromoteAvailable = true;
      //printLcd("12345678901234567890");
      printLcd("Promocja piona.",
               "Przemiana na:",
               "a-wieza, b-skoczek,",
               "c-goniec, d-hetman");
      ledEnemysMove(true, false); //prewencyjnie
    }
    else if (strDataReceived.substring(0,8) == "GameOver")
    {
      String strWhoWon = strDataReceived.substring(9);
      if (strWhoWon == "white_won")
      //printLcd("12345678901234567890");
        printLcd("Koniec gry. Oczywi-",
                 "scie wygral Igor, tj",
                 "biale. Wcisnij START",
                 "by zaczac nowa gre");
      else if (strWhoWon == "black_won")
      //printLcd("12345678901234567890");
        printLcd("CRITICAL ERROR: Ko-",
                 "niec gry: czarne wy-",
                 "graly. Wcisnij START",
                 "by zaczac nowa gre");
      else if (strWhoWon == "draw")
      //printLcd("12345678901234567890");
        printLcd("ERROR: Koniec gry:",
                 "Remis. REBOOTING...",
                 "Wcisnij START, by",
                 "zaczac nowa gre");
      else Serial.print("ERROR: wrong strWhoWon parameter");

      //TODO: tu skończyłem...
    }
    else //fukcja serwisowa: echo back
    {
      String strDataToSend = "@echo: " + strDataReceived + "$";
      Serial.print(strDataToSend);
    }
    bCoreFullDataBlock = false; //wiadomość otrzymana. wyczyść flagę
    strDataReceived = "";
  }

  if (bPromoteAvailable == true) //TODO: pod nowe przyciski to ogarnąć
  {
    if (strPromotePiece = "0") //zabezpieczenie przed mozliwascia kolejnego wcisniecia podczas potwierdzania promocji
      strPromoteType = promote(); //czekaj az gracz wcisnie przycisk odpowiadajacy promocji
    if (strPromoteType != "0")
    {
      if (strPromoteType == "a") strPromotePiece = "wieze";
      else if (strPromoteType == "b") strPromotePiece = "skoczka";
      else if (strPromoteType == "c") strPromotePiece = "gonca";
      else if (strPromoteType == "d") strPromotePiece = "hetmana";
      //printLcd("12345678901234567890");
      printLcd("Potwierdz promocje",
               "na " + strPromotePiece,
               "przyciskiem WYSLIJ");
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
      if (chBtnType == 't') //start button
      {
        //jeżeli nie jesteśmy w fazie startu gry (rozgrywka trwa) i gracz chce zresetować rozgrywkę
        if (bGameJustStarted == false && bConfirmedReset == false)
        {
          //printLcd("12345678901234567890");
          printLcd("Czy chcesz",
                   "zresetowac gre?");
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
        //printLcd("12345678901234567890");
          printLcd("Rozpoczynanie gry...");
          Serial.print("@doFirstIgorMove$"); //gra ruszyła, niech AI wykona swój pierwszy ruch
        }
      }
      else if (chBtnType == 'd' && bSendAvailable == true) //send można wysłać tylko wtedy, gdy jest gotowy ruch do wysłania
      {
        if (strPromotePiece == "0") //jezeli to nie jest promocja
        {
          printLcd("Wyslano ruch: " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo);
          Serial.print("@move " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo + "$"); //move e2e4
        }
        else //jezeli potwierdzamy promocje
        {
          //printLcd("12345678901234567890");
          printLcd("Pion zostal",
                   "promowany na " + strPromotePiece);
          //"promowany na hetmana"
          bPromoteAvailable = false; //koniec sytuacji promocji
          strPromotePiece = "0"; //czyscimy by nie wpadac w warunki promocji
          Serial.print("promote_to " + strPromoteType);
          strPromoteType = "0"; //czyszczenie by nie wpadac w warunek pytania o promocje
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
        clearMove();
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

void printLcd(String strLcdMsg1)
{
  lcd.clear();
  lcd.print(strLcdMsg1);
  lcd.setCursor(0, 0);
}

void printLcd(String strLcdMsg1, String strLcdMsg2)
{
  lcd.clear();
  lcd.print(strLcdMsg1);
  lcd.setCursor(0, 1);
  lcd.print(strLcdMsg2);
  lcd.setCursor(0, 0);
}

void printLcd(String strLcdMsg1, String strLcdMsg2, String strLcdMsg3)
{
  lcd.clear();
  lcd.print(strLcdMsg1);
  lcd.setCursor(0, 1);
  lcd.print(strLcdMsg2);
  lcd.setCursor(0, 2);
  lcd.print(strLcdMsg3);
  lcd.setCursor(0, 0);
}

void printLcd(String strLcdMsg1, String strLcdMsg2, String strLcdMsg3, String strLcdMsg4)
{
  lcd.clear();
  lcd.print(strLcdMsg1);
  lcd.setCursor(0, 1);
  lcd.print(strLcdMsg2);
  lcd.setCursor(0, 2);
  lcd.print(strLcdMsg3);
  lcd.setCursor(0, 3);
  lcd.print(strLcdMsg4);
  lcd.setCursor(0, 0);
}

String lettersButtonsActivated(char chTargetField)
{
  if (bPrintOnce == false)
  {
    if (chTargetField == 'f') //dla przycisków 'from'
    {
      ledEnemysMove(true, false);

      if (!bBadMove)
      {
        printLcd("Wybierz bierke ktora",
                 "chcesz wykonac ruch",
                 "(przyciski A-H)");
      }
      else
      {
        //printLcd("12345678901234567890");
        printLcd("Nieprawidlowy ruch!",
                 "Wybierz bierke ktora",
                 "chcesz wykonac ruch",
                 "(przyciski A-H)");
        bBadMove = false;
      }

      //jeszcze nic gracz nie wpisał, więc przycisk WYCZYSC jest ciagle nieaktywny
    }
    else if (chTargetField == 't') //dla przycisków 'to'
    {
      bClearAvailable = true; //jest co czyścić
      digitalWrite(nLedClear, HIGH); //dioda mówi nam o tym że można poprawić żądanie ruchu
      ledEnemysMove(false, true);
      //printLcd("12345678901234567890");
      printLcd("Wybierz pole na",
               "ktore chcesz",
               "przemiescic bierke",
               "(przyciski A-H)");
    }
    else Serial.print(" -ERROR: wrong 'chTargetField' value - ");

    bPrintOnce = true;
  }

  String result = "";
  switch (customKey)
  {
    case 'a': result = "a"; break;
    case 'b': result = "b"; break;
    case 'c': result = "c"; break;
    case 'd': result = "d"; break;
    case 'e': result = "e"; break;
    case 'f': result = "f"; break;
    case 'g': result = "g"; break;
    case 'h': result = "h"; break;
  }
  if (result != "")
  {
    if (chTargetField == 'f') strGameStatus = "selectDigitFrom";
    else if (chTargetField == 't') strGameStatus = "selectDigitTo";
    bPrintOnce = false;
    //digitalWrite(nLedTest, HIGH); //testowa dioda
    return result;
  }
}

int digitsButtonsActivated(char chTargetField)
{
  if (bPrintOnce == false)
  {
    bClearAvailable = true; //jest co czyścić

    if (chTargetField == 'f') //dla przycisków 'from'
      printLcd("Wybierz bierke ktora",
               "chcesz wykonac ruch",
               "(przyciski 1-8)");
    else if (chTargetField == 't') //dla przycisków 'to'
      printLcd("Wybierz pole na",
               "ktore chcesz",
               "przemiescic bierke",
               "(przyciski 1-8)");
    else Serial.print(" -ERROR: wrong 'chTargetField' value - ");

    bPrintOnce = true;
  }

  int result = 0;
  switch (customKey)
  {
    case '1': result = 1; break; //return nie jest jednocześnie breakiem?
    case '2': result = 2; break;
    case '3': result = 3; break;
    case '4': result = 4; break;
    case '5': result = 5; break;
    case '6': result = 6; break;
    case '7': result = 7; break;
    case '8': result = 8; break;
  }
  if (result > 0)
  {
    if (chTargetField == 'f') strGameStatus = "selectLetterTo";
    else if (chTargetField == 't') strGameStatus = "moveIsReady";
    bPrintOnce = false;
    return result;
  }
}

String promote()
{
  if (customKey == 'a') return "a";
  else if (customKey == 'b') return "b";
  else if (customKey == 'c') return "c";
  else if (customKey == 'd') return "d";
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

void clearMove()
{
  strGameStatus = "selectLetterFrom";
  strLetterFrom = "-1";
  strLetterTo = "-1";
  nDigitFrom = -1;
  nDigitTo = -1;
  bClearAvailable = false; //wyczyszczone. nie da się od razu ponownie wcisnąć przycisku WYCZYSC
  digitalWrite(nLedClear, LOW);
  ledEnemysMove(false, false); //zgaś obie diody, które pokazują czy teraz wbijany był ruch skąd, czy dokąd
}

