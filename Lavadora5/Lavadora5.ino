
#define ZERO 2
#define TACHO 3
#define MOTOR 4
#define NIVEL A5
#define START A1
#define ATRAS 6
#define Entrada1 7
#define Entrada2 8
#define DESAGUE 5

#define pulsosPorVuelta 72
// number of triac trigger pulse width counts. One count is 16 microseconds
#define PULSE 2
#define nivelLavar 590                              
#define nivelCale 580 
                            
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

/********PID********/
#include <PID_v1.h>
//Define Variables we'll be connecting to
volatile double velocidad, rpm, Output, Tcond;
volatile double pidError;
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
//int velocidad;
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
  /**
  unsigned long tiempo = micros() - anterior;
  if(tiempo>1000000){
    rpm=0;
    contador=0; 
    }
    else{
        double segundos  = ((double)tiempo) / 1000000;
        double prerpm = 60 / segundos;
        double mediaRpm = prerpm / pulsosPorVuelta;     
        media= media+mediaRpm;
        if(contador>9){
          rpm=media/10;
          contador=0;
          media=0;
          }      
      }
  anterior = micros();
  **/
  } 


/*****************/
/***** SETUP *****/
/*****************/

void setup(){
  //ratioPolea=50;
  tiempoInicio=millis();
  Paso=Lavado;
  Serial.begin(115200);

  
  /********PID********/
  //initialize the variables we're linked to
  rpm=0;
  //velocidadMotor=0;
  myPID.SetOutputLimits(Tmin, Tmax); 
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1);
  /********PID********/
 /********RPM********/  
  anterior=millis();
  contador=0; 
  velocidad=0;
  pinMode(TACHO, INPUT);
  attachInterrupt(digitalPinToInterrupt(TACHO), _RPM, RISING);
  /********RPM********/
  
  /********ZERO********/
  pinMode(ZERO, INPUT);
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
  /********ZERO********/
  
  /********MOTOR********/
  pinMode(MOTOR, OUTPUT);
  pinMode(ATRAS, OUTPUT);
  Tcond=Tmax;
  // set up Timer1
  OCR1A = 100;                        // initialize the comparator
  TIMSK1 = 0x03;                      // enable comparator A and overflow interrupts
  TCCR1A = 0x00;                      // timer control registers set for
  TCCR1B = 0x00;                      // normal operation, timer disabled
  /********MOTOR********/

/*******NIVEL*******/
  pinMode(NIVEL, INPUT);
  pinMode(Entrada1, OUTPUT);
  pinMode(DESAGUE, OUTPUT);
  digitalWrite(Entrada1,LOW);
  digitalWrite(Entrada2,LOW);
  digitalWrite(DESAGUE,LOW);
  PcInt::attachInterrupt(NIVEL, pinChanged, "Pin has changed to ", CHANGE);
  ultimoCambio=micros();
  ahoraCambio=0;
/*******NIVEL*******/
  
  }

void loop(){
      //botón de start 
    if(parado){
      do{
        Serial.println("Esperando Start");
        delay (30);
        tiempoInicio=millis();
        anteriorTic=0;
      }while(digitalRead(START)== LOW);
    }
    /**
    if(millis()-anteriorTic>=1000){
      tic++;
      anteriorTic=0;
      }
    **/  
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
  //Serial.print("tiempoPrograma");
  //Serial.println(tiempoPrograma);
  //Serial.print("tic");
  //Serial.println(tic);

    startflag=true;
    tiempoPaso=12;
    if(tiempoPrograma<720000){
      Lavar();
      }
    else if(tiempoPrograma<780000){
      Paso=Desague2;
      Desague();
      }
    else if(tiempoPrograma<900000){
      Paso=Aclarado1;
      Lavar();
      }
    else if(tiempoPrograma<960000){
      Paso=Desague3;
      Desague();
      }
    else if(tiempoPrograma<1080000){
      Paso=Aclarado2;
      Lavar();
      }
    else if(tiempoPrograma<1140000){
      Paso=Desague4;
      Desague();
      }
    else if(tiempoPrograma<1200000){
      Paso=Aclarado3;
      Lavar();
      }

    else if(tiempoPrograma<1320000){
      Paso=Centrifugado;
      Desague();
      }
    else if(tiempoPrograma<1800000){
      parado=true;
      digitalWrite(DESAGUE, LOW);
      }
    //myPID.Compute();
}
void Lavar(){      
    /*******NIVEL*******/
    int frecuenciaNivel=nivelCale;
    if(Paso==Lavado){frecuenciaNivel=nivelLavar;}
    digitalWrite(DESAGUE, LOW);
    //leer nivel lavar  
    //float frecuencia=1000000/periodoNivel;
    if(frecuencia>frecuenciaNivel){
       digitalWrite(Entrada1,HIGH);
       if(Paso==Aclarado3){digitalWrite(Entrada2,HIGH);}
      }
      else{
        digitalWrite(Entrada1,LOW);
      }
    /*******NIVEL*******/
    //velocidad=40;    
    //startflag=true;
    //calcularD(); 

    //Serial.print("Tcond  : ");
    //Serial.println(Tcond);
    //Serial.print("Output  : ");
    //Serial.println(Output);
    //Serial.print("Velocidad: ");
    //Serial.println(velocidad);
    Serial.print("RPM: ");
    Serial.println(rpm);
    //Serial.print("Nivel: ");
    //Serial.println(nivel);
    //Serial.print("tic: ");
    //Serial.println(tic);
    /****/
    if(tic<12){
      startflag=true;
      digitalWrite(ATRAS, LOW);
      velocidad=25;
      calcularD();
      }
    else if(tic<16){
      startflag=false;
      velocidad=0;
      calcularD();
      }      
    else if(tic<17){
      startflag=false;
      velocidad=0;
      digitalWrite(ATRAS, HIGH);
      }      
    else if(tic<28){
      startflag=true;
      velocidad=25;
      digitalWrite(ATRAS, HIGH);
      //
      calcularD();
        //  
      }  
    else if(tic<32){
      startflag=false;
      velocidad=0;
      calcularD();
      }
    else if(tic<33){
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
        if(Paso=Centrifugado){
          /**
          velocidad=40;
          calcularD();
          delay(2000);
          velocidad=80;
          calcularD();
          delay(2000);
          velocidad=160;
          calcularD();
          delay(2000);
          velocidad=320;
          calcularD();
          delay(2000);
          **/
          velocidad=1000;
          calcularD();
          }
        else{
          velocidad=50;
          calcularD();
          }
    }
  
 void calcularD(){
  pidError=rpm-velocidad;
  if(pidError>40){digitalWrite(MOTOR, LOW);}
  myPID.SetOutputLimits(0, 100);
  //myPID.SetTunings(0.1,1, 0);
  
/********PID********/
    //Input = rpm;
    //myPID.Compute();
    //Output=Output;
/********PID********/
if(startflag){
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
  if(pidError<0){
    myPID.SetTunings(0.09,0, 0);
    Tcond=map(Output,0,100,80,600);
    }else{
      myPID.SetTunings(0.9,0,0);
      Tcond=map(Output,0,100,250,600);
      }
    myPID.Compute();
    } 
else{detachInterrupt(digitalPinToInterrupt(ZERO));}
 }
/*
  myPID.SetTunings(0.1,1, 0);
  myPID.Compute();

if(rpm<1){
        myPID.SetControllerDirection(REVERSE);
        myPID.SetOutputLimits(300, Tmax);
        myPID.SetTunings(0.01,0.2, 0);
        velocidadMotor=10;
        myPID.Compute();
        }
        else{
          myPID.SetControllerDirection(REVERSE);
          myPID.SetOutputLimits(260, Tmax);
          myPID.SetTunings(0.1, 1, 0);
          velocidadMotor=300;
          myPID.Compute();
          }

  }
 */ 


 
