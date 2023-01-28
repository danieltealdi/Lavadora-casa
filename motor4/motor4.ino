
#define MOTOR3 11
#define MOTOR2 10
#define MOTOR1 9
#define REVERSE 8
#define RPM 3
#define NIVEL 7
#define START 5
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
#include  <TimerOne.h> 

//volatile int i=0;               // Variable to use as a counter volatile as it is in an interrupt
//volatile boolean zero_cross=0;
//int vProg=40;
//volatile int rpm;
//int velocidad = 200;           // Dimming level (0-50)  0 = on, 50= 0ff
volatile unsigned long anterior;
volatile unsigned long ahora;
unsigned long tiempo;

volatile unsigned long contador;                                  // tacho pulse
long freqStep = 100000;    // 5 pasos en un segundo
int Paso=Lavado;
volatile int tic=0;
int Mas;
int Menos;
float nivel;
volatile float rpm;
volatile boolean Giro;
     
  

void setup(){ 
  pinMode(RPM, INPUT_PULLUP); // Definimos el pin 3 como entrada configurada en pull-up por software.                              // El pin 2 nos vale para realizar interrupciones por hardware en Arduino Uno.
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(REVERSE, OUTPUT);
  pinMode(NIVEL, INPUT);
  pinMode(START, INPUT);
  Serial.begin(9600);                            // Establecemos comunicación con el puerto serie para poder monitorizar los resultados.
  attachInterrupt(1, _RPM, FALLING) ;
  Timer1.initialize(freqStep);                      // Initialize TimerOne library 
  Timer1.attachInterrupt(inversor, freqStep);
  contador=0;
  rpm=0;
  anterior=0;
  ahora=0;
  tiempo=0;
  do{
    Serial.println("Esperando Start");
    delay (300);
   
  }while(digitalRead(START)== LOW);
}

void loop(){
  Mas=pulseIn(NIVEL, HIGH);
  Menos=pulseIn(NIVEL, LOW);
  nivel=1000000/(Mas+Menos);
  //Mas=pulseIn(RPM, HIGH, 42000);
  //Menos=pulseIn(RPM, LOW, 42000);
  //rpm=1000000/(Mas);
  //ahora=millis();
  //tiempo=ahora-anterior;
  //anterior=ahora;
  //rpm=contador;
  //contador=0;
   //reset rpm after motor stops
  if (contador == 0 and Giro == false) {
    rpm = 0;
  }
  Serial.print("RPM:               ");
  Serial.println(rpm);
  delay(1000);}


void inversor(){
  int unSegundo=1000000/freqStep;
  //unsigned long tacho;
  float velocidad;
  int Motor;
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
      Motor=0;
      }
    else if(tic<16*unSegundo){
      Giro=true;
      Reverse=false;
      Motor=3;}
    else if(tic<20*unSegundo){
      Giro=false;
      Reverse=true;
      Motor=0;}
    else if(tic<32*unSegundo){
      Giro=true;
      Reverse=true;
      Motor=3;}
    
    else {
      Giro=false;
      Reverse=false;
      Motor=0;
      }
    }
  else{
      Giro=true;
      Reverse=false;}
  
  if (Paso==1 or Paso==3 or Paso==5 or Paso==7){
        velocidad=120; //100rpm
        Motor=3;}
  if(Paso==9){
    velocidad=960; //800rpm
    Motor=3;}
  
  if(Reverse){digitalWrite(REVERSE, LOW);}
  else{digitalWrite(REVERSE, HIGH);}
  //Serial.print("REVERSE: ");
  //Serial.println(Reverse);
  if(rpm>velocidad){Motor=0;}
  if(Giro and (rpm<velocidad)){
    Motor=3;}  
    if (Motor==0){
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
     // Serial.println("Motor0: ");
      }
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

  
//void _RPM(){                                 // FUnción que se activa cada vez que se detecta un cero en el pin 3
  //Serial.println("int");
//  contador=contador+1;
//  }
// RPM counting routine
void _RPM(){
  contador++;
  unsigned long time = micros() - anterior;
  float time_in_sec  = ((float)time) / 1000000;
  float prerpm = 60 / time_in_sec;
  rpm = prerpm / pulsosPorVuelta;
  anterior = micros();
  Serial.println(rpm);
}
  /**
  //detachInterrupt(0) ;
  unsigned long cuenta=contador;
  contador=0;
    //attachInterrupt(0, Zero_Cross, FALLING) ;
  ahora=millis();
  tiempo=(ahora-anterior)/1000;
  anterior=ahora;
  
  rpm=cuenta*tiempo*100;
  
  Serial.print("cuenta: ");
  Serial.println(cuenta);
  Serial.print("tiempo: ");
  Serial.println(tiempo);
  Serial.print("rpm: ");
  Serial.println(rpm);

  
  
  if (rpm<vProg){
    velocidad=velocidad-10;
    if (velocidad<10){velocidad=0;}
    }
  else{velocidad=velocidad+50;
      if (velocidad>190){velocidad=200;}
      }
  Serial.print(velocidad);
}

void Zero_Cross()                             // FUnción que se activa cada vez que se detecta un cero en el pin 2
  {
      zero_cross = true;               
      i=0;
      digitalWrite(MOTOR, LOW);       // turn off TRIAC (and AC)
  }

// Turn on the TRIAC at the appropriate time
void dim_check() { 
                    
    if(zero_cross == true) {              
      if(i>=velocidad) {                     
        digitalWrite(MOTOR, HIGH); // turn on light       
        i=0;  // reset time step counter                         
        zero_cross = false; //reset zero cross detection
      } 
      else {
        i++; // increment time step counter                     
      }                                
    }
    }
 **/                                                                     
