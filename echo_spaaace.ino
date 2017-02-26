int received_data;
char buffer;
String data;

const int buttonPin1 = 2;
const int buttonPin2 = 3;
int buttonState1 = 0;
int buttonState2 = 0;
bool bWroteOnce1 = false;
bool bWroteOnce2 = false;

//todo: zamiast podciągać w dół rezystorami mogę podciągać w górę programowo...
//...

void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  received_data = 1;

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);

  digitalWrite(13, LOW);
}

void loop()
{
  serialEvent();

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  if (buttonState1 && !bWroteOnce1)
  {
    Serial.write("@buttonState1 == HIGH$"); //println dodawałby '\r\n' co psuje '$'
    bWroteOnce1 = true;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  else if (!buttonState1 && bWroteOnce1)
  {
    //Serial.println("buttonState1 == LOW");
    bWroteOnce1 = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }

  if (buttonState2 && !bWroteOnce2)
  {
    Serial.write("@buttonState2 == HIGH$");
    bWroteOnce2 = true;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }
  else if (!buttonState2 && bWroteOnce2)
  {
    //Serial.println("buttonState2 == LOW");
    bWroteOnce2 = false;
    delay(20); //zabezpieczenie przed wibrowaniem przycisku przy wciskaniu
  }

  if (received_data == 0)
  {
    Serial.write("@echo$");
    digitalWrite(13, HIGH); //zaświeć diodą
    delay(2000);
    digitalWrite(13, LOW);
    received_data = 1;
  }
}

void serialEvent()
{
  while (Serial.available() > 0)
  {
    buffer = Serial.read();
    if (buffer == '$') {
      received_data = 0;
      break;
    }
    data += buffer;
  }
}
