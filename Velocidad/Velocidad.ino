#define TACHO 2
#define MOTOR 13
#define LENTO 12
#define LAVAR 5
#define DISTRI 8000
#define CENTI 80000

int contador = 0;                                    // Contador de pulsos = de pasos por cero de la corriente alterna.
unsigned long ahora = 0;                // Variables para gestionar las medidas cada segundo y contar la suma de pulsos cada segundo.
unsigned long anterior = 0;                // Variables para gestionar las medidas cada segundo y contar la suma de pulsos cada segundo.
unsigned long TachoT = 0;    // Vamos a monitorizar la suma de pulsos cada segundo (1000 milisegundos).
boolean lento=true;
  
void setup()
   {   pinMode(TACHO, INPUT_PULLUP); // Definimos el pin 2 como entrada configurada en pull-up por software.
       pinMode(MOTOR, OUTPUT);                          
                                                  // El pin 2 nos vale para realizar interrupciones por hardware en Arduino Uno.
       Serial.begin(9600);                            // Establecemos comunicación con el puerto serie para poder monitorizar los resultados.
       attachInterrupt(0, Zero_Cross, FALLING) ;     // Un cero hace que se active la función Zero_Cross que suma uno al contador.
   }

void loop() 
{
  if(lento){digitalWrite(LENTO, HIGH);}
  lento=false;
} 
 
    
    void Zero_Cross()                             // FUnción que se activa cada vez que se detecta un cero en el pin 2
       {    
        //Serial.println(anterior);
        ahora=millis();
         //Serial.println(ahora);
  
            TachoT=ahora-anterior;
            anterior=millis();
            Serial.println(TachoT);
             if (TachoT>LAVAR){
    digitalWrite(LENTO, HIGH);
    //delay(1);
    //digitalWrite(LENTO, HIGH);
    }else{
      digitalWrite(LENTO,LOW);}
     delay (3);  
}
 
