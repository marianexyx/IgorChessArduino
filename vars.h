const int NONE = 0;
const int MOVE_IS_READY = 1;
const int MOVE_WAITS_TO_BE_SEND = 2;
const int STARTED = 3;
const int SELECT_LETTER_FROM = 4;
const int SELECT_DIGIT_FROM = 5;
const int SELECT_LETTER_TO = 6;
const int SELECT_DIGIT_TO = 7;
const int BAD_MOVE = 8;
const int PROMOTE = 9;
const int ARDUINO_READY = 10;
const int PRESS_START = 11;
const int PROMOTE_TO = 12;
const int WHITE_WON = 13;
const int BLACK_WON = 14;
const int DRAW = 15;
const int CONFIRM_MOVE = 16;
const int CONFIRM_PROMOTION = 17;
const int STARTING = 18;
const int SENT_MOVE = 19;
const int PROMOTED = 20;
const int RESET = 21;

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


//todo: zamiast podciągać w dół rezystorami mogę podciągać w górę programowo...
