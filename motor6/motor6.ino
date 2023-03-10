
#define ZERO 2
#define TACHO 3
#define MOTOR 11
// number of triac trigger pulse width counts. One count is 16 microseconds
#define PULSE 4
#define NIVEL 7
#define START 5
#define ATRAS 6
#define nivelLavar 590                              
#define nivelCale 580 
#define Entrada1 12
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
/********PID********/
#include <PID_v1.h>
//Define Variables we'll be connecting to
volatile double velocidad, rpm, Tcond;
//Specify the links and initial tuning parameters
//PID myPID(&rpm, &Tcond, &velocidad,0.25,0.2,0,P_ON_E, REVERSE); //P_ON_M specifies that Proportional on Measurement be used
                                                            //P_ON_E (Proportional on Error) is the default behavior
PID myPID(&rpm, &Tcond, &velocidad,2,12,.1,P_ON_E, REVERSE); //P_ON_M specifies that Proportional on Measurement be used
                                                            //P_ON_E (Proportional on Error) is the default behavior
/********PID********/

unsigned long freqStep=1000;
volatile unsigned long tic=0;
boolean Giro;
//int velocidad;
int Paso;
volatile unsigned long contador;
//volatile int rpm;
//Tcond 80-520 max. vel.- min. vel.
//volatile int Tcond;
int Tmax=520;
int Tmin=80;
unsigned long ahora;
unsigned long anterior;
boolean startflag = false;
unsigned long Mas;
unsigned long Menos;
unsigned long nivel;
boolean parado=true;

void setup(){
  
  /********PID********/
  //initialize the variables we're linked to
  rpm=0;
  velocidad=0;
  myPID.SetOutputLimits(290, Tmax); 
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  /********PID********/
  
  // Initialize MsTimer2 library
  //MsTimer2::set(freqStep, inversor);
  //MsTimer2::start();
  anterior=millis();
  contador=0; 
  velocidad=0;
  Paso=Lavado;
  pinMode(ZERO, INPUT);
  pinMode(TACHO, INPUT);
  pinMode(NIVEL, INPUT);
  attachInterrupt(digitalPinToInterrupt(TACHO), _RPM, RISING);
  pinMode(MOTOR, OUTPUT);
  pinMode(ATRAS, OUTPUT);
  pinMode(Entrada1, OUTPUT);
  digitalWrite(Entrada1,HIGH);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
  Tcond=Tmax;
  // set up Timer1
  OCR1A = 100;                        // initialize the comparator
  TIMSK1 = 0x03;                      // enable comparator A and overflow interrupts
  TCCR1A = 0x00;                      // timer control registers set for
  TCCR1B = 0x00;                      // normal operation, timer disabled
  
  }

void loop(){
    delay(1000);
    //bot??n de start 
    if(parado){
      do{
        Serial.println("Esperando Start");
        delay (30);
       
      }while(digitalRead(START)== LOW);
    }
    parado=false;
    //startflag=true;
 /**   
    //leer nivel lavar 680 
    Mas=pulseIn(NIVEL, HIGH);
    Menos=pulseIn(NIVEL, LOW);
    nivel=1000000/(Mas+Menos);
    do{
    //if(nivel<nivelLavar){
      digitalWrite(Entrada1,LOW);
      }while (nivel<nivelLavar);
    **/  
    digitalWrite(Entrada1,HIGH);
    
    calcularD();
    velocidad=18;
    tic++;
    Serial.print("startflag: ");
    Serial.println(startflag);
    Serial.print("Tcond  : ");
    Serial.println(Tcond);
    Serial.print("Velocidad: ");
    Serial.println(velocidad);
    Serial.print("RPM: ");
    Serial.println(rpm);
    Serial.print("Nivel: ");
    Serial.println(nivel);

    if(tic<12){
      startflag=true;
      digitalWrite(ATRAS, HIGH);
      }
    else if(tic<16){
      startflag=false;
      digitalWrite(ATRAS, LOW);
      }      
    else if(tic<28){
      startflag=true;
      digitalWrite(ATRAS, LOW);
      }  
    else if(tic<32){
      startflag=false;
      digitalWrite(ATRAS, HIGH);
      }
    else{tic=0;}
    
    }
  
 void calcularD(){
  /**
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
  **/


/********PID********/
    //Input = rpm;
    myPID.Compute();
    //Tcond=Output;
/********PID********

  
  if (rpm<velocidad){
    Tcond=Tcond-10;
    if (Tcond<Tmin){Tcond=Tmin;}
    }
    else{
      Tcond=Tcond+10;
      if(Tcond>Tmax){Tcond=Tmax;}
      }
  */
  }
  

void Zero_Cross(){
  TCCR1B = 0x04;               // start timer with divide by 256 input
  TCNT1 = 0;                   // reset timer - count from zero
  OCR1A = Tcond;             // set the compare register brightness desired.
  } 
  

ISR(TIMER1_COMPA_vect) {       // comparator match
  if (startflag == true) {     // flag for start up delay
    digitalWrite(MOTOR, HIGH);  // set TRIAC gate to high
    TCNT1 = 65536-PULSE;     // trigger pulse width
    }
  }

ISR(TIMER1_OVF_vect) {         // timer1 overflow
  digitalWrite(MOTOR, LOW);     // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
  }
void _RPM(){                                 // FUnci??n que se activa cada vez que se detecta un cero en el pin 3
  contador++;
  //Serial.println(contador);
  unsigned long tiempo = micros() - anterior;
  if(tiempo>100000000){
    rpm=0;
    contador=0;
    }
    else{
        float segundos  = ((float)tiempo) / 1000000;
        float prerpm = 60 / segundos;
        float mediaRpm = prerpm / 80;     
        float media= media+mediaRpm;
        if(contador>9){
          contador=0;
          rpm=media/10;
          }      
      }

  anterior = micros();
  }

 /**
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
  
  if(Reverse){digitalWrite(ATRAS, LOW);}
  else{digitalWrite(ATRAS, HIGH);}
  //Serial.print("ATRAS: ");
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
      
   
  }

**/
