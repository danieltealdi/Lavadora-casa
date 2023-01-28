#define TACHO 2
#define ZERO 3
#define MOTOR 13
#include <TimerOne.h>

volatile byte Contador=0;
unsigned long anterior;  // Variable para guardar el tiempo de la ultima interrupción del tacómetro.
unsigned long ahora;  // Variable para guardar el tiempo de la ultima interrupción del tacómetro.
volatile unsigned long TachoT;    // Variable para guardar el tiempo de la interrupción del tacómetro.
volatile boolean vale;
unsigned long contador;
int rpm;

int velocidad;
unsigned long Tcond;


void setup()
{ 
  Timer1.initialize(500000);  // 0.5s
  Timer1.attachInterrupt(_RPM);
  velocidad=50;
  Tcond=0; //milisegundos de 0 a 10;

  
  
  pinMode(TACHO, INPUT_PULLUP); // Definimos el pin 2 como entrada configurada en pull-up por software.
  pinMode(ZERO, INPUT_PULLUP); // Definimos el pin 2 como entrada configurada en pull-up por software.                              // El pin 2 nos vale para realizar interrupciones por hardware en Arduino Uno.
  anterior = 0;
  ahora=0;
  TachoT=0;
  contador=0;
  rpm=0;
  Serial.begin(9600);                            // Establecemos comunicación con el puerto serie para poder monitorizar los resultados.
  attachInterrupt(0, Zero_Cross, FALLING) ;      // Un cero hace que se active la función Zero_Cross que suma uno al contador.
  attachInterrupt(1, _RPM, FALLING) ;      // Un cero hace que se active la función Zero_Cross que suma uno al contador.
}

void loop()
{
  delay(1000);
  //detachInterrupt(0) ;
  //contador=TachoT;
  Serial.print("RPM:     ");
  
  Serial.println(rpm);
  //Serial.println(anterior);
  //contador=ahora-anterior;
  //Serial.println(contador);
  //anterior=millis();
  
  //attachInterrupt(0, Zero_Cross, FALLING) ;
  //int rpm=60000/contador;
  //Serial.println(rpm);
  //contador=0;
  //contador = TachoT;
  //vale=true;
}

void Zero_Cross()                             // FUnción que se activa cada vez que se detecta un cero en el pin 2
{ //TachoT = millis()-anterior;
  //anterior = TachoT;
  //int rpm=60000/TachoT;
  //Serial.println(rpm);
  //vale=true;
  digitalWrite(MOTOR, HIGH);
  delay(Tcond);
  digitalWrite(MOTOR, LOW);
  contador++;
  //Serial.println(contador);
}
void _RPM(){                                 // FUnción que se activa cada vez que se detecta un cero en el pin 3
  detachInterrupt(0) ;
  unsigned long cuenta=contador;
  contador=0;
  attachInterrupt(0, Zero_Cross, FALLING) ;
  rpm=cuenta*120/80;
  if (rpm<velocidad){
    Tcond++;
    if (Tcond>9){Tcond=10;}
    }
  else{Tcond--;}
  
  }
