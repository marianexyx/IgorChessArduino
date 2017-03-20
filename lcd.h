#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27. lcd display: 20 chars, 4 lines

bool bPrintOnce = false;

void printLcd(int msg)
{
  switch (msg)
  {
    case ARDUINO_READY:
      //prtLcd("12345678901234567890");
      printLcd("Arduino is ready.");
      break;
    case PRESS_START:
      //prtLcd("12345678901234567890");
      printLcd("Wcisnij start, aby",
               "rozpaczac gre");
      break;
    case PROMOTE_TO:
      //prtLcd("12345678901234567890");
      printLcd("Promocja piona.",
               "Przemiana na:",
               "a-wieza, b-skoczek,",
               "c-goniec, d-hetman");
      break;
    case WHITE_WON:
      //prtLcd("12345678901234567890");
      printLcd("Koniec gry. Oczywi-",
               "scie wygral Igor, tj",
               "biale. Wcisnij START",
               "by zaczac nowa gre");
      break;
    case BLACK_WON:
      //prtLcd("12345678901234567890");
      printLcd("CRITICAL ERROR: Ko-",
               "niec gry: czarne wy-",
               "graly. Wcisnij START",
               "by zaczac nowa gre");
      break;
    case DRAW:
      //prtLcd("12345678901234567890");
      printLcd("ERROR: Koniec gry:",
               "Remis. REBOOTING...",
               "Wcisnij START, by",
               "zaczac nowa gre");
      break;
    case STARTING:
      //prtLcd("12345678901234567890");
      printLcd("Rozpoczynanie gry...");
      break;
    case BAD_MOVE:
      //prtLcd("12345678901234567890");
      printLcd("Nieprawidlowy ruch!",
               "Wybierz bierke ktora",
               "chcesz wykonac ruch",
               "(przyciski A-H)");
      break;
    case SELECT_LETTER_FROM:
      //prtLcd("12345678901234567890");
      printLcd("Wybierz bierke ktora",
               "chcesz wykonac ruch",
               "(przyciski A-H)");
      break;
    case SELECT_DIGIT_FROM:
      //prtLcd("12345678901234567890");
      printLcd("Wybierz bierke ktora",
               "chcesz wykonac ruch",
               "(przyciski 1-8)");
      break;
    case SELECT_LETTER_TO:
      //prtLcd("12345678901234567890");
      printLcd("Wybierz pole na",
               "ktore chcesz",
               "przemiescic bierke",
               "(przyciski A-H)");
      break;
    case SELECT_DIGIT_TO:
      //prtLcd("12345678901234567890");
      printLcd("Wybierz pole na",
               "ktore chcesz",
               "przemiescic bierke",
               "(przyciski 1-8)");
      break;
    case RESET:
      //prtLcd("12345678901234567890");
      printLcd("Czy chcesz",
               "zresetowac gre?");
      break;
    default:
      //prtLcd("12345678901234567890");
      printLcd("ERROR: Unknown",
               "printLcd parameter.");
      break;
  }
}

void printLcd(int msg, String str)
{
  switch (msg)
  {
    case CONFIRM_PROMOTION:
      //prtLcd("12345678901234567890");
      printLcd("Potwierdz promocje",
               "na " + str,
               "przyciskiem WYSLIJ");
      break;
    case SENT_MOVE:
      //prtLcd("12345678901234567890");
      //prtLcd("Wyslano ruch: e2e4");
      printLcd("Wyslano ruch: " + str);
      break;
    case PROMOTED:
      //prtLcd("12345678901234567890");
      printLcd("Pion zostal",
               "promowany na " + str);
      //......."promowany na hetmana"
      break;
    default:
      //prtLcd("12345678901234567890");
      printLcd("ERROR: printLcd(int",
               "msg, String str): ",
               "uknown msg parameter")
      break;
  }
}

void printLcd(int msg, String str1, String str2)
{
  if (msg == MOVE_IS_READY)
  {
    //prtLcd("12345678901234567890");
    printLcd("Chcesz wykonac ruch",
             "z pola " + str1 + " na " + str2 + ".",
             //"z pola e2 na e4."
             "Potwierdz: WYSLIJ,",
             "lub popraw: WYCZYSC");
  }
  else
  {
    //prtLcd("12345678901234567890");
    printLcd("ERROR: printLcd",
             "msg parameter !=",
             "MOVE_IS_READY")
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


