
int contador = 0;

void setup(){
  pinMode(2, INPUT); 
  Serial.begin(9600);
  attachInterrupt(0, Zero_Cross, RISING);
  }

void loop(){
 Serial.println(contador);
 delay(1000);
 } 
    
void Zero_Cross(){
  contador++;
  }
 
