//FUNCTIONS
bool button(bool bBtnState, bool bWriteOnce)
{
  if (bBtnState && !bWriteOnce) //wciskany przycisk uruchomi funkcję raz mimo długiego trzymania
  {
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
    bBtnStartOnce = button(startBtnState, startBtnState);
  }
  if (bSend)
  {
    sendBtnState = digitalRead(sendBtnPin);
    bBtnSendOnce = button(sendBtnState, sendBtnState);
  }
  if (bClear)
  {
    clearBtnState = digitalRead(clearBtnPin);
    bBtnClearOnce = button(clearBtnState, clearBtnState);
  }
  if (bBtnsA_H || bBtns1_8 || bBtnsA_D)
  {
    customKey = customKeypad.getKey(); //odczytywany wciskany przycisk
  }
}

void skladajPrzychodzaceDane()
{
  while (Serial.available() > 0)
  {
    buffer = Serial.read();
    if (buffer == '$') 
    {
      bCoreFullDataBlock = true;
      break;
    }
    strDataReceived += buffer;
  }
}

/*void ledWhoseTurn() //led informujący gracza o tym czy może się ruszać (TODO: zamienić na "czekaj..." or smtg?)
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
  }*/

void clearMove()
{
  strLetterFrom = "-1";
  strLetterTo = "-1";
  nDigitFrom = -1;
  nDigitTo = -1;
  strPromoteType = "-1";
  strPromotePiece = "-1";
}

void sendToCoreOnce(int nGmSt)
{
  String strMsg = "";
  
  if (!nPrinted)
  {
    if (nGmSt == CORE_START) strMsg = "@doFirstIgorMove$";
    else if (nGmSt == CORE_MOVE) strMsg = "@move " + strLetterFrom + (String)nDigitFrom +
                                             strLetterTo + (String)nDigitTo + "$";
    else if (nGmSt == CORE_PROMOTE) strMsg = "@promoteTo " + strPromoteType + "$";
    else if (nGmSt == CORE_RESET) strMsg = "@reset$";
    else strMsg = "@ERROR: Unknown gameStatus() parameter$";

    Serial.print(strMsg);
    nPrinted = true; //zamieni się na false gdy jakąś otrzymamy całą zmienną z core
  }
}

void gameStatus()
{
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
      sendToCoreOnce(nGameStatus); //gra ruszyła, niech AI wykona swój pierwszy ruch
      break;
    case SELECT_LETTER_FROM:
      interfaceAvailable(1, 0, 0, 1, 0, 0);
      strLetterFrom = lettersButtonsActivated(); //wprowadzenie litery pola skąd
      if (bBtnStartOnce) {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (strLetterFrom != "-1") nGameStatus = SELECT_DIGIT_FROM;
      break;
    case SELECT_DIGIT_FROM:
      interfaceAvailable(1, 0, 1, 0, 1, 0);
      nDigitFrom = digitsButtonsActivated(); //wprowadzenie cyfry pola skąd
      if (bBtnStartOnce) {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (bBtnClearOnce) nGameStatus = CLEAR_MOVE;
      if (nDigitFrom > 0) nGameStatus = SELECT_LETTER_TO;
      break;
    case SELECT_LETTER_TO:
      interfaceAvailable(1, 0, 1, 1, 0, 0);
      strLetterTo = lettersButtonsActivated(); //wprowadzenie litery pola skąd
      if (bBtnStartOnce) 
      {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (bBtnClearOnce) nGameStatus = CLEAR_MOVE;
      if (strLetterTo != "-1") nGameStatus = SELECT_DIGIT_TO; //wprowadzenie cyfry pola dokąd
      break;
    case SELECT_DIGIT_TO:
      interfaceAvailable(1, 0, 1, 0, 1, 0);
      nDigitTo = digitsButtonsActivated(); //wprowadzenie cyfry pola skąd
      if (bBtnStartOnce) 
      {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (bBtnClearOnce) nGameStatus = CLEAR_MOVE;
      if (nDigitTo > 0) nGameStatus = MOVE_IS_READY;
      break;
    //void interfaceAvailable(bool bStart, bool bSend, bool bClear, bool bBtnsA_H, bool bBtns1_8, bool bBtnsA_D)
    case MOVE_IS_READY:
      interfaceAvailable(1, 1, 1, 0, 0, 0);
      //digitalWrite(nLedSend, HIGH);
      if (bBtnStartOnce) 
      {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (bBtnSendOnce) nGameStatus = CORE_MOVE;
      if (bBtnClearOnce) nGameStatus = CLEAR_MOVE;
      break;
    case CORE_MOVE:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      sendToCoreOnce(nGameStatus); //move e2e4
      clearMove();
      break;
    case BAD_MOVE:
      clearMove(); //prewencyjnie
      interfaceAvailable(1, 0, 0, 1, 0, 0);
      strLetterFrom = lettersButtonsActivated(); //wprowadzenie litery pola skąd
      if (bBtnStartOnce) 
      {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (strLetterFrom != "-1") nGameStatus = SELECT_DIGIT_FROM;
      break;
    case ARDUINO_PROMOTE:
      interfaceAvailable(1, 0, 0, 0, 0, 1);
      //ledEnemysMove(true, false); //prewencyjnie
      strPromoteType = promoteButtonsActivated();
      if (bBtnStartOnce) {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (strPromoteType != "-1") nGameStatus = CONFIRM_PROMOTION;
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
    case CONFIRM_PROMOTION:
      interfaceAvailable(1, 1, 1, 0, 0, 0);
      if (bBtnStartOnce) {
        nPrevGmStatus = nGameStatus;
        nGameStatus = RESET;
      }
      if (bBtnSendOnce) nGameStatus = CORE_PROMOTE;
      if (bBtnClearOnce) nGameStatus = CLEAR_PROMOTE;
      break;
    case CORE_PROMOTE:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      sendToCoreOnce(nGameStatus);  //"promoteTo d"
      clearMove();
      break;
    case RESET:
      interfaceAvailable(0, 1, 1, 0, 0, 0);
      if (bBtnSendOnce) nGameStatus = CORE_RESET;
      if (bBtnClearOnce) nGameStatus = nPrevGmStatus;
      break;
    case CORE_RESET:
      interfaceAvailable(0, 0, 0, 0, 0, 0);
      sendToCoreOnce(nGameStatus); 
      clearMove();
      break;
    case CLEAR_MOVE:
      clearMove();
      nGameStatus = SELECT_LETTER_FROM;
      break;
    case CLEAR_PROMOTE:
      clearMove();
      nGameStatus = ARDUINO_PROMOTE;
      break;
    default:
      //prtLcd("12345678901234567890");
      printLcd("ERROR: Unknown",
               "gameStatus()",
               "parameter");
      sendToCoreOnce(nGameStatus);
      break;
  }
}

void sprawdzajPrzychodzaceDane()
{
  if (bCoreFullDataBlock == true) //odczytywanie danych po serialu z core'a
  {
    if (strDataReceived == "started") nGameStatus = STARTED;
    else if (strDataReceived == "IgorHasEndedMove" || strDataReceived == "PromotedTo") nGameStatus = SELECT_LETTER_FROM;
    else if (strDataReceived.substring(0, 8) == "BAD_MOVE") nGameStatus = BAD_MOVE;
    else if (strDataReceived == "promote") nGameStatus = ARDUINO_PROMOTE;
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

    nPrinted = false;
  }
}

