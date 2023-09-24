
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
/**#define velLavar 3
#define kp 0.3
#define ki 0.01
//#define kd 0

#define tPre 6
#define tDes 5
#define tLav 20
#define tAcl 6
#define tCen1 0
#define tCen2 6

#define tPre 0
#define tDes 2
#define tLav 0
#define tAcl 0
#define tCen1 0
#define tCen2 6
**/ 
/*Tiempo de cada paso del programa*/
/***********
//LAVADO NORMAL
  int tPre=6;
  int tDes=5;
  int tLav=20;
  int tAcl=6;
  int tCen1=0;
  int tCen2=6;
                       
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
//volatile int rpm;
//Tcond 20-625 max. vel.- min. vel.
//volatile int Tcond;
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
/*****************/
/***** ISR *****/
/*****************/

/*******NIVEL*******/
void pinChanged(const char* message, bool pinstate) {
  ahoraCambio=micros();
  periodoNivel=2*(ahoraCambio-ultimoCambio);
  ultimoCambio=micros();
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

void _RPM(){                                 // FUnción que se activa cada vez que se detecta un cero en el pin 3
  //contador++;
  periodo=micros()-lastflash;
  lastflash=micros();
  } 


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
  //attachInterrupt(digitalPinToInterrupt(TACHO), _RPM, RISING);
 
  //!!!!!!!!!!!!aqui está el cambio!!!!!!!!!!!!!
  
  
  /********RPM********/
  
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
  Serial.print("tiempoPrograma");
  Serial.println(tiempoPrograma);
  Serial.print("tic");
  Serial.println(tic);

    //startflag=true;
    //tiempoPaso=12;
    if(tiempoPrograma<tLav*60000){
      Serial.println("Lavar");
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
void Lavar(){      
    /*******NIVEL*******/
    int frecuenciaNivel=nivelCale;
    if(Paso==Prelavado or Paso==Lavado){frecuenciaNivel=nivelLavar;}
    digitalWrite(DESAGUE, LOW);
    if(frecuencia>frecuenciaNivel){
       if(Paso==Prelavado){
        digitalWrite(Entrada1,HIGH);
        digitalWrite(Entrada2,LOW);
        }
        else if(Paso==Aclarado3){
          digitalWrite(Entrada1,HIGH);
          digitalWrite(Entrada2,HIGH);
          }
          else{
          digitalWrite(Entrada2,HIGH);
          digitalWrite(Entrada1,LOW);
          }
      }
      else{
        digitalWrite(Entrada1,LOW);
        digitalWrite(Entrada2,LOW);
      }
    /*******NIVEL*******/
    //Serial.print("Tcond  : ");
    //Serial.println(Tcond);
    //Serial.print("Output  : ");
    //Serial.println(Output);
    //Serial.print("Velocidad: ");
    //Serial.println(velocidad);
    //Serial.print("RPM: ");
    //Serial.println(rpm);
    //Serial.print("Nivel: ");
    //Serial.println(nivel);
    //Serial.print("tic: ");
    //Serial.println(tic);
    /****/
    if(tic<1){
      startflag=true;
      digitalWrite(ATRAS, LOW);
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<2){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<3){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<4){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<5){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<6){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<7){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<12){
      startflag=true;
      velocidad=velLavar;
      calcularD();
      }
    else if(tic<15){
      startflag=false;
      velocidad=0;
      calcularD();
      }      
    else if(tic<16){
      startflag=false;
      velocidad=0;
      digitalWrite(ATRAS, HIGH);
      }      
    else if(tic<17){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<18){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<19){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<20){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<21){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<22){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<23){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<28){
      startflag=true;
      velocidad=velLavar;
      digitalWrite(ATRAS, HIGH);
      calcularD();  
      }  
    else if(tic<31){
      startflag=false;
      velocidad=0;
      calcularD();
      }
    else if(tic<32){
      startflag=false;
      velocidad=0;
      digitalWrite(ATRAS, LOW);
      }
    else{
      tic=0;
      startflag=true;}
      
}
void Desague(){
        startflag=true;
        digitalWrite(ATRAS, LOW);
        digitalWrite(DESAGUE, HIGH);
        digitalWrite(Entrada1, LOW); 
        digitalWrite(Entrada2, LOW); 
        if(Paso==Centrifugado1){
          velocidad=350;
          calcularD();
          }
        else if(Paso==Centrifugado2){
          velocidad=1000;
          calcularD();
          }
        else{
          velocidad=15;
          calcularD();
          }
    }
  
 void calcularD(){
  pidError=rpm-velocidad;
  //if(pidError>80){digitalWrite(MOTOR, LOW);}
  myPID.SetOutputLimits(0, 100);

if(startflag){
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
  /**
  if(pidError<0){
    myPID.SetTunings(1,0, 0);
    Tcond=map(Output,0,100,80,600);
    }else{
      myPID.SetTunings(1,0,0);
      Tcond=map(Output,0,100,80,600);
      }
    myPID.Compute();
    }
    **/
    myPID.SetTunings(kp,ki,kd);
    if (Paso==Centrifugado2){
      Tcond=map(Output,0,100,80,600);
      }
      else{
        Tcond=map(Output,0,100,240,625);
      }
    myPID.Compute();
}    
else{detachInterrupt(digitalPinToInterrupt(ZERO));}
}



 
