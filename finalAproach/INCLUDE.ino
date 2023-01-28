void valores(){
  //inicial pk=0.7 ik=0.05 dk=0
  pk=0.2;
  ik=0;
  dk=0;
  //inicial Pre=6 Lav=12 Des=2 Acl=5 Cen=5
  tPre=6;
  tDes1=0;
  tLav=12;
  tDes2=5;
  tAcl1=0;
  tDes3=0;
  tAcl2=0;
  tDes4=0;
  tAcl3=0;
  tDes5=0;
  tCen=0;
  nivelLavar=600;
  nivelCale=580;
  Tmax=620;
  Tmin=455;
  velocidadLavar=25;
  velocidadCentrifugar=1000;
  velocidadDesaguar=50;
}
void calcularD(){
  pidError=rpm-velocidad;
  if(pidError>80){digitalWrite(MOTOR, LOW);}
  myPID.SetOutputLimits(0, 100);

  if(startflag){
    //attachInterrupt(digitalPinToInterrupt(ZERO), Zero_Cross, RISING);
    if(pidError<0){
      myPID.SetTunings(pk,ik, dk);
      Tcond=map(Output,0,100,Tmin,Tmax);
      }else{
        myPID.SetTunings(pk,ik,dk);
        Tcond=map(Output,0,100,Tmin,Tmax);
        }
      myPID.Compute();
      } 
  //else{detachInterrupt(digitalPinToInterrupt(ZERO));}
  }
