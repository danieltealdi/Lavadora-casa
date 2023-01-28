
#define MOTOR3 11
#define MOTOR2 10
#define MOTOR1 9
#define REVERSE 8
#define RPM 3                              
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
//unsigned long anterior;
//unsigned long ahora;
//unsigned long tiempo;

volatile unsigned long contador;                                  // tacho pulse
long freqStep = 200000;    // 5 pasos en un segundo
int Paso=Lavado;
volatile int tic=60;


void setup(){ 
  pinMode(RPM, INPUT_PULLUP); // Definimos el pin 3 como entrada configurada en pull-up por software.                              // El pin 2 nos vale para realizar interrupciones por hardware en Arduino Uno.
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(REVERSE, OUTPUT);
  Serial.begin(9600);                            // Establecemos comunicación con el puerto serie para poder monitorizar los resultados.
  attachInterrupt(1, _RPM, FALLING) ;
  Timer1.initialize(freqStep);                      // Initialize TimerOne library 
  Timer1.attachInterrupt(inversor, freqStep);   
  //contador=0;
  //rpm=0;
  //anterior=0;
  //ahora=0;
  //tiempo=0;
}

void loop(){
  Serial.print("Contador:               ");
  Serial.println(contador);
  delay(1000);}


void inversor(){
  unsigned long tacho;
  unsigned long velocidad;
  int Motor;
  boolean Reverse;
  boolean Giro;
  tacho=contador;  
  contador=0;
  tic++;
  if (Paso%2==0){
    velocidad=16;
    
    if(tic>156){tic=0;}
    if(tic<2){
      Giro=true;
      Reverse=false;
      Motor=2;
      }
    else if(tic<3){
      Giro=true;
      Reverse=false;
      Motor=3;}
    else if(tic<15){
      Giro=true;
      Reverse=false;
      Motor=2;}
    else if(tic<56){
      Giro=true;
      Reverse=false;
      Motor=1;}
    else if(tic<76){
      Reverse=false;
      Giro=false;
      Motor=0;
      velocidad=0;}
    else if(tic<78){
      Reverse=true;
      Giro=true;
      Motor=2;
      }
    else if(tic<79){
      Reverse=true;
      Giro=true;
      Motor=3;
      }
    else if(tic<91){
      Reverse=true;
      Giro=true;
      Motor=2;
      }
    else if(tic<136){
      Reverse=true;
      Giro=true;
      Motor=1;
      }
    else {
      Giro=false;
      Reverse=false;
      Motor=0;
      velocidad=0;}
    }
  else{
      Giro=true;
      Reverse=false;}
  
  if (Paso==1 or Paso==3 or Paso==5 or Paso==7){
        velocidad=48;
        Motor=2;}
  if(Paso==9){
    velocidad=300;
    Motor=3;}
  
  if(Reverse){digitalWrite(REVERSE, LOW);}
  else{digitalWrite(REVERSE, HIGH);}
  Serial.print("REVERSE: ");
  Serial.println(Reverse);
  if(tacho>velocidad){Motor=0;}
  if(tacho<velocidad){
    Motor=2;
    delay(100);
    Motor=3;}  
    if (Motor==0){
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
      Serial.println("Motor0: ");
      }
    if (Motor==1){
      digitalWrite(MOTOR1, HIGH);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
      Serial.println("Motor1: ");
      //Serial.println(cuenta);
      }
    if (Motor==2){
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, HIGH);
      digitalWrite(MOTOR3, LOW);
      Serial.println("          Motor2: ");
      }
    if (Motor==3){
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, HIGH);
      Serial.println("                                    Motor3: ");
      }
      //Serial.println(tic);
  }

  
void _RPM(){                                 // FUnción que se activa cada vez que se detecta un cero en el pin 3
  contador=contador+1;
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
