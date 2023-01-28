#include <TimerOne.h>

volatile byte Contador=0;
volatile boolean Lavar=false;
//Saliodas digitales
//#define FORWARD 22
#define REVERSE 13
#define MOTORWASH 12
//#define DISTRIBUTION 24
//#define LOWEXT 25
//#define HIGHEXT 26
//#define COLD 27
//#define WARM 28
//#define PREWASH 29
#define WASH 10
//#define BLEACH 31
//#define SOFTNER 32
#define DRAIN 11
//#define LOCK 34
//#define HEAT 35
//Entradas digitales
//#define DOOR 35
//#define DOORLOCK 36
//#define TILT 37
//#define OVERLOAD 38
#define NIVEL A1

void setup(){
  Timer1.initialize(11000000);  // 1s
  Timer1.attachInterrupt(Inversor);
  attachInterrupt(0, Zero_Cross, FALLING) 
//  Serial.begin(9600);
  pinMode(WASH, OUTPUT);
  pinMode(DRAIN, OUTPUT);
//  pinMode(NIVEL,)
  pinMode(REVERSE, OUTPUT);
  pinMode(MOTORWASH, OUTPUT);
  digitalWrite(REVERSE, HIGH);
  digitalWrite(REVERSE, HIGH);
  
}
void Inversor(void){
 // Serial.println("entrada");
  if (Lavar){
    if (Contador<11){
      digitalWrite(REVERSE, HIGH);
      delay(10);
      digitalWrite(MOTORWASH, LOW);
    }
    else if (Contador<15){
      digitalWrite(MOTORWASH, HIGH);
    }
    else if (Contador<27){
      digitalWrite(REVERSE, LOW);
      delay(10);
      digitalWrite(MOTORWASH, LOW);
    }
    else if (Contador<31){
      digitalWrite(MOTORWASH, HIGH);
    }
//    Serial.println(Contador);

    Contador=Contador+1;
//    Serial.println(Contador);

    if(Contador>31){
      Contador=0;
    }
  }
}


void loop(){
//  byte ContadorCopy;
//  noInterrupts();
//  ContadorCopy = Contador;
//  interrupts();
  //Lavado
  Lavar=true;
  int Nivel=analogRead(NIVEL);
  Serial.println(Nivel);
  while(Nivel<900){
    digitalWrite(WASH, LOW);
    }
    digitalWrite(WASH, HIGH);
 
//  Serial.println("ContadorCopy: ");
//  Serial.println(ContadorCopy);

    delay(1000);
 }
