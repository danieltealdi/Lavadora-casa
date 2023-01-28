
#define ZERO 2
#define TACHO 3
#define MOTOR 4
#define NIVEL A5
#define START 5
#define ATRAS 6
#define Entrada1 7
#define Entrada2 8
#define DESAGUE 9

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
volatile double velocidad, rpm, Tcond;
//Specify the links and initial tuning parameters
//PID myPID(&rpm, &Tcond, &velocidad,0.25,0.2,0,P_ON_E, REVERSE); //P_ON_M specifies that Proportional on Measurement be used
                                                            //P_ON_E (Proportional on Error) is the default behavior
//PID myPID(&rpm, &Tcond, &velocidad,2,12,.1,P_ON_E, REVERSE);
//PID myPID(&rpm, &Tcond, &velocidad,0.25,12,0,P_ON_E, REVERSE);
PID myPID(&rpm, &Tcond, &velocidad,0.7,4096,0,P_ON_E, REVERSE);


/********PID********/
/*******NIVEL*******/
#include <YetAnotherPcInt.h>
volatile unsigned long ultimoCambio;
volatile unsigned long ahoraCambio;
volatile unsigned long periodoNivel;
/*******NIVEL*******/

unsigned long freqStep=1000;
volatile unsigned long tic=0;
boolean Giro;
//int velocidad;
int Paso;
volatile unsigned long contador;
//volatile int rpm;
//Tcond 80-520 max. vel.- min. vel.
//volatile int Tcond;
int Tmax=390;
int Tmin=300;
unsigned long ahora;
unsigned long anterior;
boolean startflag = false;
unsigned long Mas;
unsigned long Menos;
boolean nivel=false;
boolean parado=true;
unsigned long tiempoInicio;
unsigned long tiempoPrograma;
unsigned long tiempoPaso;

/*******NIVEL*******/
void pinChanged(const char* message, bool pinstate) {
  ahoraCambio=micros();
  Serial.println(ahoraCambio);
  periodoNivel=2*(ahoraCambio-ultimoCambio);
  Serial.println(periodoNivel);
  ultimoCambio=ahoraCambio;
 }
/*******NIVEL*******/

void setup(){
  tiempoInicio=millis();
  Paso=Lavado;
  Serial.begin(115200);

  
  /********PID********/
  //initialize the variables we're linked to
  rpm=0;
  velocidad=0;
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
  digitalWrite(Entrada1,HIGH);
  digitalWrite(DESAGUE,HIGH);
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
      }while(digitalRead(START)== LOW);
    }
    
    parado=false;
    Paso=Centrifugado;
   // tiempoPrograma=millis();
  //  delay(1000);
  while(millis()<(tiempoPrograma+1000)){}
  tiempoPrograma=millis()-tiempoInicio;
  //Serial.print("tiempoPrograma");
  //Serial.println(tiempoPrograma);
  //Serial.print("tic");
  //Serial.println(tic);
/**
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
      Paso=Desague3;
      Desague();
      }
    else if(tiempoPrograma<1188000){
      Paso=Centrifugado;
      Desague();
      }
    else if(tiempoPrograma<1668000){
      parado=true;
      digitalWrite(DESAGUE, HIGH);
      }
      **/
      Desague();
}
void Lavar(){      
    /*******NIVEL*******/
    int frecuenciaNivel=nivelCale;
    if(Paso=Lavado){frecuenciaNivel=nivelLavar;}
    digitalWrite(DESAGUE, HIGH);
    //leer nivel lavar  
    float frecuencia=1000000/periodoNivel;
    if(frecuencia>frecuenciaNivel){
       digitalWrite(Entrada1,LOW);
      }
      else{
        digitalWrite(Entrada1,HIGH);
      }
    /*******NIVEL*******/
        
    startflag=true;


    tic++;
    /**
    Serial.print("Tcond  : ");
    Serial.println(Tcond);
    Serial.print("Velocidad: ");
    Serial.println(velocidad);
    Serial.print("RPM: ");
    Serial.println(rpm);
    //Serial.print("Nivel: ");
    //Serial.println(nivel);
    //Serial.print("tic: ");
    //Serial.println(tic);
**/
    if(tic<12){
      startflag=true;
      digitalWrite(ATRAS, HIGH);
      //
      calcularD();
        //  
      }
    else if(tic<16){
      startflag=false;
      velocidad=0;
      myPID.Compute();
      }      
    else if(tic<17){
      startflag=false;
      digitalWrite(ATRAS, LOW);
      }      
    else if(tic<28){
      startflag=true;
      digitalWrite(ATRAS, LOW);
      //
      calcularD();
        //  
      }  
    else if(tic<32){
      startflag=false;
      velocidad=0;
      myPID.Compute();
      }
    else if(tic<33){
      startflag=false;
      digitalWrite(ATRAS, HIGH);
      }
    else{
      tic=0;
      startflag=true;}
}
void Desague(){ 
        if(Paso=Centrifugado){
          velocidad=5000;
            myPID.SetOutputLimits(40, 520); 

          }
        else{velocidad=36;}
        startflag=true;
        digitalWrite(ATRAS, LOW);
        digitalWrite(DESAGUE, LOW);
        digitalWrite(Entrada1, HIGH);
        myPID.Compute();

    }
  
 void calcularD(){
  
/********PID********/
if(rpm<5){
        myPID.SetControllerDirection(REVERSE);
        myPID.SetOutputLimits(300, Tmax);
        myPID.SetTunings(0.7, 64, 0);
        velocidad=1;
        myPID.Compute();
        }
        else{
          myPID.SetControllerDirection(REVERSE);
          myPID.SetOutputLimits(240, 390);
          myPID.SetTunings(0.2, 4096, 0);
          velocidad=5;
          myPID.Compute();
          }
/********PID********/
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
  contador++;
  unsigned long tiempo = micros() - anterior;
  if(tiempo>500000){
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

 
