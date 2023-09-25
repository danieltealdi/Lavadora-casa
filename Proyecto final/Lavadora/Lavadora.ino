
#define ZERO 2
#define TACHO 3
#define MOTOR 4
#define NIVEL A5
#define PROG1 A1
#define PROG2 A2
#define ATRAS 6
#define Entrada1 8
#define Entrada2 7
#define DESAGUE 5
#define OCUPADO 9

#define pulsosPorVuelta 72
// number of triac trigger pulse width counts. One count is 16 microseconds
#define PULSE 1
#define nivelLavar 600                              
#define nivelCale 585 
/*Tiempo de cada paso del programa*/
/***********          
//CENTRIFUGADO
  int tPre=0;
  int tDes=5;
  int tLav=0;
  int tAcl=6;
  int tCen1=0;
  int tCen2=6;
*/                       
//LAVADO NORMAL
  int tPre=6;
  int tDes1=4;
  int tDes2=4;
  int tDes3=4;
  int tDes4=4;
  int tDes5=2;
  int tLav=20;
  int tAcl=5;
  int tCen1=0;
  int tCen2=6;
             
  /*/DESAGUAR Y TERMINAR
  int tPre=0;
  int tDes1=0;
  int tDes2=0;
  int tDes3=0;
  int tDes4=0;
  int tDes5=1;
  int tLav=0;
  int tAcl=0;
  int tCen1=0;
  int tCen2=0;
  /*/                    
#define Prelavado 0
#define Desague1 1
#define Lavado 2
#define Desague2 3
#define Centrifugado1 4
#define Aclarado1 5
#define Desague3 6
#define Aclarado2 7
#define Desague4 8
#define Aclarado3 9
#define Desague5 10
#define Centrifugado2 11
#define FinProg 12

#define TERMINAR 0
#define CENTRIFUGAR 1
#define RAPIDO 2
#define NORMAL 3


/********PID********/
#include <PID_v1.h>
//Define Variables we'll be connecting to
double velocidad, rpm, Output, Tcond;
double pidError;
//Specify the links and initial tuning parameters
PID myPID(&rpm, &Output, &velocidad,0.1,1,0,P_ON_E, REVERSE);
/********PID********/
/*******NIVEL*******/
#include <YetAnotherPcInt.h>
volatile unsigned long ultimoCambio;
volatile unsigned long ahoraCambio;
volatile unsigned long periodoNivel;
/*******NIVEL*******/

unsigned long freqStep=1000;
unsigned long tic=0;
unsigned long anteriorTic=0;
boolean Giro;
double velLavar;
volatile double kp;
volatile double ki;
volatile double kd;
int Paso;
volatile unsigned long contador=0;
volatile double media=0;
volatile unsigned long lastflash=0;
volatile unsigned long periodo;
int Tmax=625;
int Tmin=20;
int programa=TERMINAR;
unsigned long ahora;
unsigned long anterior=0;
boolean startflag = false;
unsigned long Mas;
unsigned long Menos;
boolean nivel=false;
boolean parado=true;
unsigned long tiempoInicio;
unsigned long tiempoPrograma;
unsigned long tiempoAnterior;
unsigned long tiempoPaso;
unsigned long medirSegundos=0;
float frecuencia;

#include "Lavadora.hpp"
/*****************/
/***** SETUP *****/
/*****************/

void setup(){
  /*A1 y A2 definen los programas 
   11:Lavado normal
   10:Lavado rápido
   01:Centrifugar
   00:desaguar y terminar*/
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (OCUPADO, OUTPUT);
  digitalWrite (OCUPADO, HIGH);
  if (digitalRead(A1)==1 && digitalRead(A2)==1){programa=NORMAL;}
  else if (digitalRead(A1)==1 && digitalRead(A2)==0){programa=RAPIDO;}
  else if (digitalRead(A1)==0 && digitalRead(A2)==1){programa=CENTRIFUGAR;}
  else if (digitalRead(A1)==0 && digitalRead(A2)==0){programa=TERMINAR;}

  pinMode(FinProg,OUTPUT);
  digitalWrite(FinProg, HIGH);
   
  parado=true;
  //ratioPolea=50;
  tiempoInicio=millis();
  
  
  Paso=Lavado;
  Serial.begin(115200);
  
  /********PID********/
  //Variablrs de control del PID del motor
  velLavar=0.001;
  kp=0.28;
  ki=0;
  kd=0;
  /* */
  rpm=0;
  myPID.SetOutputLimits(Tmin, Tmax); 
  //Activar el PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1);
  /********PID********/
  
 /********RPM********/  
  anterior=millis();
  contador=0; 
  velocidad=0;
  pinMode(TACHO, INPUT);
  
  /********ZERO********/
  pinMode(ZERO, INPUT);
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
  /********ZERO********/
  
  /********MOTOR********/
  pinMode(MOTOR, OUTPUT);
  pinMode(ATRAS, OUTPUT);
  Tcond=Tmax;
  // configurar Timer1
  OCR1A = 100;                        // iniciar el comparador
  TIMSK1 = 0x03;                      // habilitar el comparador A y las interrupciones de overflow
  TCCR1A = 0x00;                      // registros de control del timer configurados para
  TCCR1B = 0x00;                      // operación normal, timer inhabilitado
  /********MOTOR********/

/*******NIVEL*******/
  pinMode(NIVEL, INPUT);
  pinMode(Entrada1, OUTPUT);
  digitalWrite(Entrada1,LOW);
  pinMode(Entrada2, OUTPUT);
  digitalWrite(Entrada2,LOW);
  pinMode(DESAGUE, OUTPUT);
  digitalWrite(DESAGUE,LOW);
  PcInt::attachInterrupt(NIVEL, pinChanged, "Pin has changed to ", CHANGE);
  ultimoCambio=micros();
  ahoraCambio=0;
/*******NIVEL*******/
  
  }

void loop(){
  
    /**START**/ 
    if(parado){
      /*do{*/
        Serial.println("Esperando Start");
        delay (30);
        tiempoInicio=millis();
        anteriorTic=0;
      /*}while(digitalRead(START)== LOW);*/
    }
    parado=false;
    Paso=Lavado;
    
    /**RPM**/
    unsigned long rpmPulso=pulseIn(TACHO, HIGH, 50000);
    if(rpmPulso==0){rpm=2;}
    else{
      rpm=1000000/rpmPulso*60/248;
      //247 experimental deberia serpusos por vuelta que son 124
      }
    /**RPM**/
    
  tiempoPrograma=millis()-tiempoInicio;
  medirSegundos=millis()-tiempoAnterior;
  if(medirSegundos>=1000){
    tic++;
    medirSegundos=0;
    tiempoAnterior=millis();
    frecuencia=1000000/periodoNivel;
    }
  
    if(tiempoPrograma<tLav*60000){
      Paso=Lavado;
      Lavar();
      }
    else if(tiempoPrograma<(tLav+tDes2)*60000){
      Paso=Desague2;
      Desague();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1)*60000){
      Paso=Centrifugado1;
      Desague();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl)*60000){
      Paso=Aclarado1;
      Lavar();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl+tDes3)*60000){
      Paso=Desague3;
      Desague();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl+tDes3+tAcl)*60000){
      Paso=Aclarado2;
      Lavar();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl+tDes3+tAcl+tDes4)*60000){
      Paso=Desague4;
      Desague();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl+tDes3+tAcl+tDes4+tAcl)*60000){
      Paso=Aclarado3;
      Lavar();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl+tDes3+tAcl+tDes4+tAcl+tDes5)*60000){
      Paso=Desague5;
      Desague();
      }
    else if(tiempoPrograma<(tLav+tDes2+tCen1+tAcl+tDes3+tAcl+tDes4+tAcl+tDes5+tCen2)*60000){
      Paso=Centrifugado2;
      Desague();
      }
    else{
      startflag=false;
      //parado=true;
      digitalWrite(DESAGUE, LOW);
      digitalWrite(OCUPADO, LOW);
      digitalWrite(FinProg, LOW);
      }
    }



 
