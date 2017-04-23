const int ARDUINO_READY = 1;
const int STARTED = 2;
const int CORE_START  = 3;
const int SELECT_LETTER_FROM  = 4;
const int SELECT_DIGIT_FROM = 5;
const int SELECT_LETTER_TO  = 6;
const int SELECT_DIGIT_TO = 7;
const int MOVE_IS_READY = 8;
const int CORE_MOVE = 9;
const int BAD_MOVE  = 10;
const int ARDUINO_PROMOTE = 11;
const int WHITE_WON = 12;
const int BLACK_WON = 13;
const int DRAW  = 14;
const int CONFIRM_PROMOTION = 15;
const int CORE_PROMOTE  = 16;
const int CORE_RESET  = 17;
const int RESET = 18;
const int CLEAR_MOVE = 19;
const int CLEAR_PROMOTE = 20;

int nGameStatus = ARDUINO_READY;
int nPrevGmStatus = 0;

bool nPrinted; //zabezpiecznie przed wysyłaniem w pętli tej samej wiadomości na serial

//zmienne sklejające dane
bool bCoreFullDataBlock = false; //czy są jakieś dane do wysłania
char buffer;
String strDataReceived;

const int nLedTest(10);
const int nLedFrom(40), nLedTo(41), nLedSend(42), nLedClear(43), nLedPlayer(44), nLedIgor(45);

const int startBtnPin(32), sendBtnPin(33), clearBtnPin(34);
bool startBtnState(0), sendBtnState(0), clearBtnState(0);
bool bBtnStartOnce(0), bBtnSendOnce(0), bBtnClearOnce(0);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = //define the cymbols on the buttons of the keypads
{
  {'a', 'b', 'c', 'd'},
  {'e', 'f', 'g', 'h'},
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};
byte rowPins  [ROWS] = {25, 24, 23, 22}; 
byte colPins [COLS] = {29, 28, 27, 26};
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey; //odczytywany wciskany przycisk

String strLetterFrom("-1"), strLetterTo("-1");
int nDigitFrom(-1), nDigitTo(-1);
String strPromoteType = "-1";
String strPromotePiece = "-1";

//todo: zamiast podciągać w dół rezystorami mogę podciągać w górę programowo...
