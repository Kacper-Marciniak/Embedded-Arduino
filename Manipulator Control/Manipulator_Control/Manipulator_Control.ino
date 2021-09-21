#include <Servo.h>
#define S1_cmp 94
#define S2_cmp 8
#define S3_cmp 134
#define SafetyB 3
#define HomeB 2
#define LED 7

Servo serwo3;
Servo serwo2;
Servo serwo1;
int S_prev[3] = {0 + S1_cmp, 90 + S2_cmp, -90 + S3_cmp};
int S[3] = {0 + S1_cmp, 90 + S2_cmp, -90 + S3_cmp};
long time_start;
int TP = 1000;
bool SafetyMode = false;
bool ContModeMoveJ = false;
bool ContModeGo = false;
bool LEDMode = false;

void setup() {
  SerwoWrite();
  pinMode(SafetyB, INPUT_PULLUP);
  pinMode(HomeB, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  serwo3.attach(9);  //Serwomechanizm podłączony do pinu 9
  serwo2.attach(10);  //Serwomechanizm podłączony do pinu 10
  serwo1.attach(11);  //Serwomechanizm podłączony do pinu 11
  PredkoscSerwa(25);
  Serial.begin (9600);
  delay(100);
}


void loop() {
  //PredkoscSerwa(30);
  PrintAngTHETA();
  Komunikacja();
}

void Komunikacja()
{
  Serial.println();
  Serial.println("ZAKRESY: (-85 85) (0 170) (-130 45)");
  Serial.println("<Oczekuje na dane>");
  while (Serial.available() == 0)
  {
    if (digitalRead(HomeB) == LOW)
    {
      DaneTheta(0, 90, -90);
      MoveJ();
    }
  }
  if (ContModeMoveJ || ContModeGo)
  {
    String str1  = Serial.readStringUntil(' ');
    Serial.read();
    String str2 = Serial.readStringUntil(' ');
    Serial.read();
    String str3  = Serial.readStringUntil('\0');
    if (str1.toInt() == 999 && str2.toInt() == 999 && str3.toInt() == 999)
    {
      ContModeMoveJ = false;
      ContModeGo = false;
      Serial.println("Wylaczono tryb ciaglej pracy");
    }
    else
    {
      DaneTheta(str1.toInt(), str2.toInt(), str3.toInt());
      Serial.println("<Tryb ciaglej pracy>");
      if (ContModeMoveJ) MoveJ();
      else if (ContModeGo) SerwoWrite();
    }
  }
  else
  {
    String str_fnc  = Serial.readStringUntil(' ');
    str_fnc.replace("\n", "");
    str_fnc.replace("\r", "");
    if (str_fnc == "movej" || str_fnc == "go")
    {
      String str1  = Serial.readStringUntil(' ');
      Serial.read();
      String str2 = Serial.readStringUntil(' ');
      Serial.read();
      String str3  = Serial.readStringUntil('\0');
      str3.replace("\n", "");
      str3.replace("\r", "");
      DaneTheta(str1.toInt(), str2.toInt(), str3.toInt());
      if (str_fnc == "movej") MoveJ();
      else if (str_fnc == "go") SerwoWrite();
      else return 0;
    }
    else if (str_fnc == "speed")
    {
      String str1 = Serial.readStringUntil('\0');
      int str1i = str1.toInt();
      if (str1i < 1) str1i = 1;
      else if (str1i > 40) str1i = 40;
      PredkoscSerwa(str1i);
    }
    else if (str_fnc == "home")
    {
      goHome();
    }
    else if (str_fnc == "contmovej")
    {
      ContModeGo = false;
      ContModeMoveJ = true;
      Serial.println("Uruchomiono tryb ciaglej pracy - MOVEJ");

    }
    else if (str_fnc == "contgo")
    {
      ContModeMoveJ = false;
      ContModeGo = true;
      Serial.println("Uruchomiono tryb ciaglej pracy - GO");
    }
    else if (str_fnc == "sseq")
    {
      Sekwencja();
    }
    else if (str_fnc == "led")
    {
      ToggleLED();
    }
    else if (str_fnc == "safe")
    {
      if (SafetyMode)
      {
        SafetyMode = false;
        Serial.println("Wylaczono tryb bezpieczny");
      }
      else
      {
        SafetyMode = true;
        Serial.println("Uruchomiono tryb bezpieczny");
      }
    }
    else
    {
      Serial.println(str_fnc);
      Serial.println("<NIEPOPRAWNA KOMENDA>");
    };
  }
}

void Dane(int S1, int S2, int S3)
{
  S[0] = S1;
  S[1] = S2;
  S[2] = S3;
  for (int i; i < 3; i++)
  {
    if (S[i] > 180)S[i] = 180;
    else if (S[i] < 0)S[i] = 0;
  }
}

void DaneTheta(int S1, int S2, int S3)
{
  S[0] = S1 + S1_cmp;
  S[1] = S2 + S2_cmp;
  S[2] = S3 + S3_cmp;
  for (int i; i < 3; i++)
  {
    if (S[i] > 180)S[i] = 180;
    else if (S[i] < 0)S[i] = 0;
  }
}

void SerwoWrite()
{
  LEDMode = true;
  digitalWrite(LED, HIGH);
  Serial.print("Ruch bez interpolacji do: ");
  Serial.print(S[0] - S1_cmp);
  Serial.print("  ");
  Serial.print(S[1] - S2_cmp);
  Serial.print("  ");
  Serial.println(S[2] - S3_cmp);
  if (SafetyMode)
  {
    while (digitalRead(SafetyB) == HIGH) {};
  }
  serwo1.write(S[0]);
  serwo2.write(S[1]);
  serwo3.write(S[2]);
  delay(TP);
  S_prev[0] = S[0];
  S_prev[1] = S[1];
  S_prev[2] = S[2];
  LEDMode = false;
  digitalWrite(LED, LOW);
}

void MoveJ()
{
  Serial.print("Wykonaj ruch z interpolacja zlaczowa do: ");
  Serial.print(S[0] - S1_cmp);
  Serial.print("  ");
  Serial.print(S[1] - S2_cmp);
  Serial.print("  ");
  Serial.println(S[2] - S3_cmp);
  if (S[0] == S_prev[0] && S[1] == S_prev[1] && S[2] == S_prev[2])
  {
    serwo1.write(S[0]);
    serwo2.write(S[1]);
    serwo3.write(S[2]);
    return false;
  }
  LEDMode = true;
  digitalWrite(LED, HIGH);
  int N = ceil((abs(S[0] - S_prev[0]) + abs(S[1] - S_prev[1]) + abs(S[2] - S_prev[2])) / 3);
  float S1, S2, S3;
  for (int i = 0; i <= N; i++)
  {
    time_start = millis();
    S1 = S_prev[0] + float(i) / N * (S[0] - S_prev[0]);
    S2 = S_prev[1] + float(i) / N * (S[1] - S_prev[1]);
    S3 = S_prev[2] + float(i) / N * (S[2] - S_prev[2]);
    if (SafetyMode)
    {
      while (digitalRead(SafetyB) == HIGH) {};
    }
    serwo1.write(ceil(S1));
    serwo2.write(ceil(S2));
    serwo3.write(ceil(S3));
    //PrintAng();
    //PrintAmp(5);
    while (millis() - time_start <= TP) {}
  }
  S_prev[0] = S[0];
  S_prev[1] = S[1];
  S_prev[2] = S[2];
  LEDMode = false;
  digitalWrite(LED, LOW);
}

void Sekwencja()
{
  Serial.println("Wykonuje zapisaną sekwencję");
  int tmp_time = 100;
  PredkoscSerwa(20);
  goHome();
  DaneTheta(0, 0, -15);
  MoveJ();
  delay(tmp_time);
  int speedServo = 20;
  Krok(20, -45,tmp_time, speedServo);
  Krok(31, -56,tmp_time, speedServo);
  Krok(42, -67,tmp_time, speedServo);
  Krok(53, -78,tmp_time, speedServo);
  Krok(64, -89,tmp_time, speedServo);
  Krok(75, -100,tmp_time, speedServo);
  goHome();  
}

void Krok (int k1, int k2, int tmp_time, int speedservo)
{
  PredkoscSerwa(speedservo);
  DaneTheta(0, k1, k2);
  MoveJ();
  delay(tmp_time);
}

void PrintAmp(int N)
{
  float amp[3] = {0, 0, 0};
  for (int i = 0; i < N; i++)
  {
    amp[0] += float(analogRead(A1));
    amp[1] += float(analogRead(A2));
    amp[2] += float(analogRead(A3));
    delay(1);
  }
  amp[0] = amp[0] * 5.0 / 1024.0 / 0.47 / N;
  amp[1] = amp[1] * 5.0 / 1024.0 / 0.47 / N;
  amp[2] = amp[2] * 5.0 / 1024.0 / 0.47 / N;
  if (amp[0] > 2.0) amp[0] = 2.0;
  if (amp[1] > 2.0) amp[1] = 2.0;
  if (amp[2] > 2.0) amp[2] = 2.0;
  Serial.print(amp[0]);
  Serial.print("  ");
  Serial.print(amp[1]);
  Serial.print("  ");
  Serial.println(amp[2]);
}

void PrintAng()
{
  Serial.print("Obecna pozycja (Serwo):  ");
  Serial.print(serwo1.read());
  Serial.print(" ;");
  Serial.print(serwo2.read());
  Serial.print(" ;");
  Serial.println(serwo3.read());
}

void PrintAngTHETA()
{
  Serial.print("Obecna pozycja (Theta):  ");
  Serial.print(S[0] - S1_cmp);
  Serial.print(" ;");
  Serial.print(S[1] - S2_cmp);
  Serial.print(" ;");
  Serial.println(S[2] - S3_cmp);
}

void PredkoscSerwa(int V)
{
  TP = 1000 / V;
  Serial.print("Predkosc serwa: ");
  Serial.println(V);
}

void ToggleLED()
{
  if (LEDMode)
  {
    LEDMode = false;
    Serial.println("Wylaczono LED");
    digitalWrite(LED, LOW);
  }
  else
  {
    LEDMode = true;
    Serial.println("Uruchomiono LED");
    digitalWrite(LED, HIGH);
  }
}

void goHome()
{
    DaneTheta(0, 90, -90);
    MoveJ();
}
