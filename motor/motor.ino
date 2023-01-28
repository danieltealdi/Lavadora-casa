
#define ZERO 2
#define TACHO 3
#define MOTOR 11
// number of triac trigger pulse width counts. One count is 16 microseconds
#define PULSE 4
#define NIVEL 0
#define START 5
#define REVERSE 6
#define nivelLavar 590                              
#define nivelCale 580 
#define pulsosPorVuelta 72                             
#define Prelavado 0
#define Desague1 1
#define Lavado 2
#define Desague2 3
#define Aclarado1 4
#define Desague3 5
#define Aclarado2 6
#define Desague4 7
#define Aclarado3 8
#define Centrifugado 9

//#include  <TimerOne.h> 
#include  <MsTimer2.h> 

unsigned long freqStep=1000;
volatile unsigned long tic;
boolean Giro;
int velocidad;
int Paso;
volatile unsigned long contador;
volatile int rpm;
//Tcond 80-520 max. vel.- min. vel.
volatile int Tcond;
unsigned long ahora;
unsigned long anterior;
bool startflag = true;
unsigned long Mas;
unsigned long Menos;
unsigned long nivel;

void setup(){
  // Initialize MsTimer2 library
  MsTimer2::set(freqStep, inversor);
  MsTimer2::start();
  anterior=millis();
  contador=0; 
  velocidad=50;
  Paso=Prelavado;
  pinMode(ZERO, INPUT);
  pinMode(TACHO, INPUT);
  attachInterrupt(digitalPinToInterrupt(TACHO), _RPM, RISING);
  pinMode(MOTOR, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
  Tcond=520;
  // set up Timer1
  OCR1A = 100;                        // initialize the comparator
  TIMSK1 = 0x03;                      // enable comparator A and overflow interrupts
  TCCR1A = 0x00;                      // timer control registers set for
  TCCR1B = 0x00;                      // normal operation, timer disabled
  //botón de start probablemente no debería estar aqui porque cuando termine no puede empezar un nuevo ciclo 
  do{
    Serial.println("Esperando Start");
    delay (300);
   
  }while(digitalRead(START)== HIGH);
  }

void loop(){
  delay(100);
  //leer nivel lavar 680 
  Mas=pulseIn(NIVEL, HIGH);
  Menos=pulseIn(NIVEL, LOW);
  nivel=1000000/(Mas+Menos);
  
  calcularD();
  
  Serial.print("contador:   ");
  Serial.println(contador);
  Serial.print("velocidad:   ");
  Serial.println(velocidad);
  Serial.print("RPM:   ");
  Serial.println(rpm);
  Serial.print("Tcond:   ");
  Serial.println(Tcond);
  }
 void calcularD(){
  Serial.println('calcularD');
  unsigned long tiempo;
  ahora=millis();
  tiempo=ahora-anterior;
  tiempo=tiempo/60000;
  anterior=ahora;
  Serial.print("ahora:   ");
  Serial.println(ahora);
  Serial.print("anterior:   ");
  Serial.println(anterior);
  Serial.print("tiempo:   ");
  Serial.println(tiempo);
  
  rpm=contador/tiempo/80;
  contador=0;
  if (rpm<velocidad){
    Tcond--;
    if (Tcond<80){Tcond=80;}
    }
    else{
      Tcond++;
      if(Tcond>520){Tcond=520;}
      }
  }

void Zero_Cross(){
  TCCR1B = 0x04;               // start timer with divide by 256 input
  TCNT1 = 0;                   // reset timer - count from zero
  OCR1A = Tcond;             // set the compare register brightness desired.
  }

ISR(TIMER1_COMPA_vect) {       // comparator match
  if (startflag == true) {     // flag for start up delay
    digitalWrite(MOTOR, HIGH);  // set TRIAC gate to high
    TCNT1 = 65536;     // trigger pulse width
    }
  }

ISR(TIMER1_OVF_vect) {         // timer1 overflow
  digitalWrite(MOTOR, LOW);     // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
  }
void _RPM(){                                 // FUnción que se activa cada vez que se detecta un cero en el pin 3
  contador++;
  //Serial.println(contador);
  }

 void inversor(){
  
  int unSegundo=1000000/freqStep;
  //unsigned long tacho;
  //float velocidad;
  //int Motor;
  boolean Reverse;
  //tacho=contador;  
  //contador=0;
  tic++;
  if (Paso%2==0){
    velocidad=48;//40 rpm *72 pulsos/60segundos
    
    if(tic>31*unSegundo){tic=0;}
    if(tic<4*unSegundo){
      Giro=false;
      Reverse=false;
      //Motor=0;
      }
    else if(tic<16*unSegundo){
      Giro=true;
      Reverse=false;
      //Motor=3;
      }
    else if(tic<20*unSegundo){
      Giro=false;
      Reverse=true;
      //Motor=0;
      }
    else if(tic<32*unSegundo){
      Giro=true;
      Reverse=true;
      //Motor=3;
      }
    
    else {
      Giro=false;
      Reverse=false;
      //Motor=0;
      }
    }
  else{
      Giro=true;
      Reverse=false;}
  
  if (Paso==1 or Paso==3 or Paso==5 or Paso==7){
        velocidad=120; //100rpm
        //Motor=3;
        }
  if(Paso==9){
    velocidad=960; //800rpm
    //Motor=3;
    }
  
  if(Reverse){digitalWrite(REVERSE, LOW);}
  else{digitalWrite(REVERSE, HIGH);}
  //Serial.print("REVERSE: ");
  //Serial.println(Reverse);
  //if(rpm>velocidad){Motor=0;}
  //if(Giro and (rpm<velocidad)){
  //  Motor=3;}  
  //  if (Motor==0){
  //    digitalWrite(MOTOR1, LOW);
  //    digitalWrite(MOTOR2, LOW);
  //    digitalWrite(MOTOR3, LOW);
     // Serial.println("Motor0: ");
  //    }
  /**  
   *   
   if (Motor==1){
      digitalWrite(MOTOR1, HIGH);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
      //Serial.println("Motor1: ");
      //Serial.println(cuenta);
      }
    if (Motor==2){
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, HIGH);
      digitalWrite(MOTOR3, LOW);
      //Serial.println("          Motor2: ");
      }
    if (Motor==3){
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, HIGH);
      //Serial.println("                                    Motor3: ");
      }
      //Serial.println(tic);
      */
  }
