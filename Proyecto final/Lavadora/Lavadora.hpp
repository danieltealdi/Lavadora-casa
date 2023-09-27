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

void calcularD(){
  pidError=rpm-velocidad;
  myPID.SetOutputLimits(0, 100);

if(startflag){
  attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
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


void Lavar(){      
    /*******NIVEL*******/
    int frecuenciaNivel=nivelCale;
    if(Paso==Prelavado or Paso==Lavado){frecuenciaNivel=nivelLavar;}
    digitalWrite(DESAGUE, LOW);
    //frecuencia=1000000/periodoNivel;
    //se calcula en loop() cada segundo
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
  
 


 
