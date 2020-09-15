/*
 Name:    Autonomous_and_IR_controled_car.ino
 Created: 12/15/2019 4:11:28 PM
 Author: Mateusz Urbaniak
*/
#include <IRremote.h>

#define LED_CZERWONY A0
#define LED_ZIELONY A1
#define LED_BIALY 10
#define CZUJNIK_ODL1_TRIG 2
#define CZUJNIK_ODL1_ECHO 3
#define CZUJNIK_ODL2_TRIG 13
#define CZUJNIK_ODL2_ECHO 12
#define WENTYLATOR 11
#define BUMPER1 A4
#define BUMPER2 A5
#define IN1 7
#define IN2 8
#define IN3 5
#define IN4 4
#define ENA 9
#define ENB 6
#define PILOT A3
#define POM_NAPIECIA A2

// NAGLOWKI FUNKCJI
void przod();
void lewo();
void tyl();
float zmierz_napiecie();
bool kontrola_baterii();
int zmierz_odleglosc(int trig, int echo);
bool przycisk(int przycisk);
void zdalne_sterowanie();
//ZMIENNE GLOBALNE
int predkosc = 50;
const short int min_odleglosc = 50;
unsigned long previousMillis[1]; //[x] = liczba rownoleglych zadan | [0] zarezewowany na pilot
unsigned int wypelnienie_bialy_LED = 50;
unsigned short power = 1;
IRrecv irrecv(PILOT);
decode_results wynik;
bool czekanie = true;
bool bialy_mruganie = false;

void setup() // the setup function runs once when you press reset or power the board
{
  //LEDY
  pinMode(LED_CZERWONY, OUTPUT);
  pinMode(LED_ZIELONY, OUTPUT);
  pinMode(LED_BIALY, OUTPUT); //PWM
  //CZUJNIKI DZWIEKU
  pinMode(CZUJNIK_ODL1_TRIG, OUTPUT);
  pinMode(CZUJNIK_ODL1_ECHO, INPUT);
  pinMode(CZUJNIK_ODL2_TRIG, OUTPUT);
  pinMode(CZUJNIK_ODL2_ECHO, INPUT);
  //SILNIKI
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); //PWM
  pinMode(ENB, OUTPUT); //PWM
  //PRZYCISKI
  pinMode(BUMPER1, INPUT_PULLUP); // Prawy
  pinMode(BUMPER2, INPUT_PULLUP); //Lewy
  //PILOT
  irrecv.enableIRIn(); // Zacznij odbieranie
}

void loop()
{
  if (czekanie){
    delay(500);
    czekanie = false;
  }
  else{
    if (kontrola_baterii()){
      digitalWrite(LED_ZIELONY, HIGH);
      zdalne_sterowanie();
      if (power == 1){
        digitalWrite(LED_CZERWONY, LOW);
        if (przycisk(BUMPER1) || przycisk(BUMPER2)){
          int i = 0;
          for (i = 0; i < 23000; i++){
            tyl();
          }
          int j = 0;
          for (j = 0; j < 10000; j++){
            lewo();
          }
        }
        else if (zmierz_odleglosc(CZUJNIK_ODL1_TRIG, CZUJNIK_ODL1_ECHO) < 20 || zmierz_odleglosc(
          CZUJNIK_ODL2_TRIG, CZUJNIK_ODL2_ECHO) < 20){
          int i = 0;
          for (i = 0; i < 23000; i++){
            tyl();
          }
          int j = 0;
          for (j = 0; j < 10000; j++){
            lewo();
          }
        }
        else if (zmierz_odleglosc(CZUJNIK_ODL1_TRIG, CZUJNIK_ODL1_ECHO) < min_odleglosc || zmierz_odleglosc(
          CZUJNIK_ODL2_TRIG, CZUJNIK_ODL2_ECHO) < min_odleglosc){
          lewo();
        }
        else
          przod();
      }
      else{
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        analogWrite(ENA, predkosc);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        analogWrite(ENB, predkosc);
        zdalne_sterowanie();
      }

    }

    else{
      digitalWrite(LED_CZERWONY, HIGH);
      digitalWrite(LED_ZIELONY, LOW);

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, predkosc);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, predkosc);
    }
  }
}

float zmierz_napiecie()
{
  static const float naladowana = 11.64;
  static const float rozdzielczosc = 946.0;
  float odczytane_napiecie = analogRead(POM_NAPIECIA);
  float napiecie = float(odczytane_napiecie * naladowana / rozdzielczosc);
  return napiecie;
}

bool kontrola_baterii()
{
  static const float min_napiecie = 11.0;
  if (zmierz_napiecie() > min_napiecie){
    return true;
  }
  return false;
}

int zmierz_odleglosc(int trig, int echo)
{
  long czas, dystans;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  czas = pulseIn(echo, HIGH);
  dystans = czas / 58;
  return dystans;
}

void przod()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, predkosc);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, predkosc);
}

void tyl()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 50);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 50);
}

void lewo()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, (35));
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 50);
}

bool przycisk(int przycisk) //Sprawdza czy przycisk zostal wcisniety
{
  if (digitalRead(przycisk) == LOW) // przycisk wcisniety
    return true;
  return false;
}

void zdalne_sterowanie()
{
  if (millis() - previousMillis[0] >= 100){
    previousMillis[3] = millis();
    if (irrecv.decode(&wynik)){
      irrecv.resume(); // Odbierz kolejna wartosc
      if (wynik.value == 0x807FFA05) digitalWrite(LED_CZERWONY, !digitalRead(LED_CZERWONY)); //Przycisk 1
      else if (wynik.value == 0x807F7A85) digitalWrite(LED_ZIELONY, !digitalRead(LED_ZIELONY)); //Przycisk 2
      else if (wynik.value == 0x807FBA45){
        //Przycisk 3
        if (wypelnienie_bialy_LED != 0) wypelnienie_bialy_LED = 0;
        else wypelnienie_bialy_LED = 200;
        analogWrite(LED_BIALY, wypelnienie_bialy_LED);
      }
      else if (wynik.value == 0x807FF20D){
        //Przycisk 4
        wypelnienie_bialy_LED += 50;
        if (wypelnienie_bialy_LED >= 255) wypelnienie_bialy_LED = 20;
        Serial.println(wypelnienie_bialy_LED);
      }
      else if (wynik.value == 0x807F728D){
        //Przycisk 5
        predkosc += 50;
        if (predkosc >= 255) predkosc = 50;
      }
      else if (wynik.value == 0x807FB24D){
        //Przycisk 6
        if (power == 0) power = 1;
        else{ power = 0; }
      }
      else if (wynik.value == 0x807FC23D){
        //przycisk 7
        int i = 0;
        for (i = 0; i < 18000; i++){
          lewo();
        }
      }
      else if (wynik.value == 0x807F42BD){
        //przycisk 8
        int i = 0;
        for (i = 0; i < 25000; i++){
          przod();
        }
      }
      else if (wynik.value == 0x807F827D){
        //przycisk 9
        int i = 0;
        for (i = 0; i < 25000; i++){
          tyl();
        }
      }
    }
  }
}
