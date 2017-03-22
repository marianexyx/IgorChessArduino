//FUNCTIONS
bool button(bool bBtnState, bool bWriteOnce, char chBtnType) //ogarniaj 3 przyciski funkcyjne
{
  if (bBtnState && !bWriteOnce) //wciskany przycisk uruchomi funkcję raz mimo długiego trzymania
  {
    /*if (chBtnType == 't') //start button
    {
      //jeżeli nie jesteśmy w fazie startu gry (rozgrywka trwa) i gracz chce zresetować rozgrywkę
      if (bGameJustStarted == false && bConfirmedReset == false)
      {
        printLcd(RESET);
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
        printLcd(STARTING);
        Serial.print("@doFirstIgorMove$"); //gra ruszyła, niech AI wykona swój pierwszy ruch
      }
    }
    else if (chBtnType == 'd' && bSendAvailable == true) //send można wysłać tylko wtedy, gdy jest gotowy ruch do wysłania
    {
      if (strPromotePiece == "0") //jezeli to nie jest promocja
      {
        printLcd(SENT_MOVE,  strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo));
        Serial.print("@move " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo + "$"); //move e2e4
      }
      else //jezeli potwierdzamy promocje
      {
        printLcd(PROMOTED + strPromotePiece);
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
    }*/

    bWriteOnce = true; //cokolwiek wciśnięto, to raz to zrobiono i wykonano, mimo iż przycisk przez jakąś chwilę będzie wciśniety
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  else if (!bBtnState && bWriteOnce)
  {
    bWriteOnce = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  //jeżeli żadna zmiana nie zaszła, to zwrócone zostanie to samo co weszło, czyli nic się nie stanie
  return bWriteOnce;
}

String lettersButtonsActivated()
{
  String result = "-1";

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

  if (result == "-1") return "-1";
  else return result;
}


int digitsButtonsActivated()
{
  int result = -1;

  switch (customKey)
  {
    case '1': result = 1; break;
    case '2': result = 2; break;
    case '3': result = 3; break;
    case '4': result = 4; break;
    case '5': result = 5; break;
    case '6': result = 6; break;
    case '7': result = 7; break;
    case '8': result = 8; break;
  }

  if (result <= 0) return -1;
  else return result;
}

String promoteButtonsActivated()
{
  String result = "-1";

  switch (customKey)
  {
    case 'a': strPromotePiece = "wieze"; result = "a"; break;
    case 'b': strPromotePiece = "skoczka"; result = "b"; break;
    case 'c': strPromotePiece = "gonca"; result = "c"; break;
    case 'd': strPromotePiece = "hetmana"; result = "d"; break;
  }

  if (result == "-1") return "-1";
  else return result;
}

void interfaceAvailable(bool bStart, bool bSend, bool bClear, bool bBtnsA_H, bool bBtns1_8, bool bBtnsA_D)
{
  if (bStart)
  {
    startBtnState = digitalRead(startBtnPin);
    bBtnStartOnce = button(startBtnState, bStartOnce, 't');
  }
  if (bSend)
  {
    sendBtnState = digitalRead(sendBtnPin);
    bBtnSendOnce = button(sendBtnState, bSendOnce, 'd');
  }
  if (bClear)
  {
    clearBtnState = digitalRead(clearBtnPin);
    bBtnClearOnce = button(clearBtnState, bClearOnce, 'c');
  }
  if (bBtnsA_H || bBtns1_8 || bBtnsA_D)
  {
    customKey = customKeypad.getKey(); //odczytywany wciskany przycisk
  }
}


/*String lettersButtonsActivated(char chTargetField)
  {
  if (bPrintOnce == false)
  {
    if (chTargetField == 'f') //dla przycisków 'from'
    {
      ledEnemysMove(true, false);

      if (!bBadMove) printLcd(SELECT_LETTER_FROM);
      else
      {
        printLcd(BAD_MOVE);
        bBadMove = false;
      }
      //jeszcze nic gracz nie wpisał, więc przycisk WYCZYSC jest ciagle nieaktywny
    }
    else if (chTargetField == 't') //dla przycisków 'to'
    {
      bClearAvailable = true; //jest co czyścić
      digitalWrite(nLedClear, HIGH); //dioda mówi nam o tym że można poprawić żądanie ruchu
      ledEnemysMove(false, true);
      printLcd(SELECT_LETTER_TO);
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
  }*/

/*int digitsButtonsActivated(char chTargetField)
  {
  if (bPrintOnce == false)
  {
    bClearAvailable = true; //jest co czyścić

    if (chTargetField == 'f') //dla przycisków 'from'
      printLcd(SELECT_DIGIT_FROM);
    else if (chTargetField == 't') //dla przycisków 'to'
      printLcd(SELECT_DIGIT_TO);
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
    if (chTargetField == 'f') gameStatus(SELECT_LETTER_FROM);
    else if (chTargetField == 't') gameStatus(MOVE_IS_READY); //ruch jest gotowy do wysłanie
    bPrintOnce = false;
    return result;
  }
  }*/

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

/*void clearMove()
  {
  strLetterFrom = "-1";
  strLetterTo = "-1";
  nDigitFrom = -1;
  nDigitTo = -1;
  strPromoteType = "-1";
  strPromotePiece = "-1";
  //digitalWrite(nLedClear, LOW);
  //ledEnemysMove(false, false); //zgaś obie diody, które pokazują czy teraz wbijany był ruch skąd, czy dokąd
  }*/

void gameStatus()
{
  /*interfaceAvailable(nGameStatus);
      printLcd(nGameStatus);*/ //zawsze?
  printLcd(nGameStatus);

  switch (nGameStatus)
  {
    case ARDUINO_READY:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      break;
    case STARTED:
      interfaceAvailable(1, 0, 0, 0, 0, 0);
      if (bBtnStartOnce) nGameStatus = CORE_START;
      break;
    case CORE_START:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      Serial.print("@doFirstIgorMove$"); //gra ruszyła, niech AI wykona swój pierwszy ruch
      break;
    case SELECT_LETTER_FROM:
      interfaceAvailable(1, 0, 0, 1, 0, 0);
      strLetterFrom = lettersButtonsActivated(); //wprowadzenie litery pola skąd
      if (strLetterFrom != "-1") nGameStatus = SELECT_DIGIT_FROM;
      break;
    case SELECT_DIGIT_FROM:
      interfaceAvailable(1, 0, 1, 0, 1, 0);
      nDigitFrom = digitsButtonsActivated(); //wprowadzenie cyfry pola skąd
      if (nDigitFrom > 0) nGameStatus = SELECT_LETTER_TO;
      break;
    case SELECT_LETTER_TO:
      interfaceAvailable(1, 0, 1, 1, 0, 0);
      strLetterTo = lettersButtonsActivated(); //wprowadzenie litery pola skąd
      if (strLetterTo != "-1") nGameStatus = SELECT_DIGIT_TO; //wprowadzenie cyfry pola dokąd
      break;
    case SELECT_DIGIT_TO:
      interfaceAvailable(1, 0, 1, 0, 1, 0);
      nDigitTo = digitsButtonsActivated(); //wprowadzenie cyfry pola skąd
      if (nDigitTo > 0) nGameStatus = MOVE_IS_READY;
      break;
    //void interfaceAvailable(bool bStart, bool bSend, bool bClear, bool bBtnsA_H, bool bBtns1_8, bool bBtnsA_D)
    case MOVE_IS_READY:
      interfaceAvailable(1, 1, 1, 0, 0, 0);
      //digitalWrite(nLedSend, HIGH);
      if (bBtnSendOnce) nGameStatus = CORE_MOVE;
      if (bBtnClearOnce) nGameStatus = CLEAR_MOVE;
      break;
    case CORE_MOVE:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      Serial.print("@move " + strLetterFrom + (String)nDigitFrom + strLetterTo + (String)nDigitTo + "$"); //move e2e4
      clearMove();
      break;
    case BAD_MOVE:
      clearMove(); //prewencyjnie
      interfaceAvailable(1, 0, 0, 1, 0, 0);
      strLetterFrom = lettersButtonsActivated(); //wprowadzenie litery pola skąd
      if (strLetterFrom != "-1") nGameStatus = SELECT_DIGIT_FROM;
      break;
    case WHITE_WON:
      interfaceAvailable(1, 0, 0, 0, 0, 0);
      if (bBtnStartOnce) nGameStatus = STARTED;
      break;
    case BLACK_WON:
      interfaceAvailable(1, 0, 0, 0, 0, 0);
      if (bBtnStartOnce) nGameStatus = STARTED;
      break;
    case DRAW:
      interfaceAvailable(1, 0, 0, 0, 0, 0);
      if (bBtnStartOnce) nGameStatus = STARTED;
      break;
    case ARDUINO_PROMOTE:
      interfaceAvailable(1, 0, 0, 0, 0, 1);
      //ledEnemysMove(true, false); //prewencyjnie
      strPromoteType = promoteButtonsActivated();
      if (strPromoteType != "-1") nGameStatus = PROMOTE_TO;
      break;
    case CONFIRM_PROMOTION:
      interfaceAvailable(1, 1, 1, 0, 0, 0);
      if (bBtnSendOnce) nGameStatus = CORE_PROMOTE;
      if (bBtnClearOnce) nGameStatus = CLEAR_PROMOTE;
      break;
    case CORE_PROMOTE:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      Serial.print("@promoteTo " + strPromoteType + "$"); //"promoteTo d"
      clearMove();
      break;
    case CORE_RESET:

      break;
    case RESET:

      break;
    case CLEAR_MOVE:
      strLetterFrom = "-1";
      strLetterTo = "-1";
      nDigitFrom = -1;
      nDigitTo = -1;
      nGameStatus = SELECT_LETTER_FROM;
      break;
    case CLEAR_PROMOTE:
      strPromoteType = "-1";
      strPromotePiece = "-1";
      nGameStatus = ARDUINO_PROMOTE;
      break;
    default:

      break;
  }
}

