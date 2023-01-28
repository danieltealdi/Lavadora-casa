
#define ZERO 2
#define MOTOR 11
#define REVERSE 9

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
#include  <TimerOne.h>          // Avaiable from http://www.arduino.cc/playground/Code/Timer1
volatile int i=0;               // Variable to use as a counter volatile as it is in an interrupt
volatile boolean zero_cross=0;
volatile unsigned long contador;                                  // Boolean to store a "switch" to tell us if we have crossed zero
int vProg=40;
volatile int rpm;
int velocidad = 200;           // Dimming level (0-50)  0 = on, 50= 0ff
int freqStep = 50;    // This is the delay-per-velocidad step in microseconds.
unsigned long anterior;
unsigned long ahora;
unsigned long tiempo;
boolean Reverse=false;
int Paso=Lavado;
int tic=0;
boolean Giro=false;
void setup()
  { 
  pinMode(ZERO, INPUT_PULLUP); // Definimos el pin 2 como entrada configurada en pull-up por software.                              // El pin 2 nos vale para realizar interrupciones por hardware en Arduino Uno.
  pinMode(MOTOR, OUTPUT);
  pinMode(REVERSE, OUTPUT);
  Serial.begin(9600);                            // Establecemos comunicación con el puerto serie para poder monitorizar los resultados.
  attachInterrupt(0, Zero_Cross, RISING) ;      // Un cero hace que se active la función Zero_Cross que suma uno al contador.
  attachInterrupt(1, _RPM, FALLING) ;
  Timer1.initialize(freqStep);                      // Initialize TimerOne library 
  Timer1.attachInterrupt(dim_check, freqStep);   
  contador=0;
  rpm=0;
  anterior=0;
  ahora=0;
  tiempo=0;
}




void loop()
{
  tic++;
  if (Paso%2==0){
    
    if(tic>32){tic=0;}
    if(tic<10){
      Giro=true;
      Reverse=false;}
    else if(tic<17){
      Reverse=false;
      Giro=false;
      velocidad=200;}
    else if(tic<25){
      Reverse=true;
      Giro=true;
      }
    else {
      Giro=false;
      Reverse=false;
      velocidad=200;}
    }
  else{
      Giro=true;
      Reverse=false;}
      //if(Giro){digitalWrite(GIRO, LOW);}
      //else{digitalWrite(GIRO, HIGH);}
 if(Reverse){digitalWrite(REVERSE, LOW);}
   else{digitalWrite(REVERSE, HIGH);}

  delay(1000);

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
                                                                      
void _RPM(){                                 // FUnción que se activa cada vez que se detecta un cero en el pin 3
  contador=contador+1;
  }
