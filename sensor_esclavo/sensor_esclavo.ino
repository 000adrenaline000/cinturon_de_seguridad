
//variables para guardar los datos medidos
int asientoReading;
int brocheReading;
int cinturonReading; 

//variables de conversion de las magnitudes medidas
float asientoVoltage;
float asientoResistance;
float cinturonVoltaje;
float cinturonResistance;

//numero de asientos sensados por arduino nano, maximo 4
const int n=2;

//PINES SENSORES e INDICADORES {1,2,....,n}
int asiento[n] = {A0,A2};  //sensor para el asiento
int cinturon[n]= {A1,A3}; //sensor para el cinturon
int broche[n]  = {11,8};   //sensor de broche 
int leds[n]    = {10,7};     //led alarma de uso incorrecto del cinturon de seguridad
int buzzer[n]  = {9,6};    //buzzer alarma de uso incorrecto del cinturon de seguridad

//variables auxiliares del programa
int Estado[n]={0,0};
int flag_Estado[n]={0,0};

//dato a enviar
int alarma[n];

unsigned long lastTime[n];  // para almacenar el tiempo actual en que se detecta el uso incorrecto del cinturon
int tiempo_espera=10000; //5 segundos, tiempo en que el sistema debe esperar a que el usuario haga el uso correcto del cinturon
                                      //antes de que se active las alarmas

//conexion serial
const int EnTxPin =  2;  // HIGH:TX y LOW:RX
const int mydireccion =101; //Direccion del esclavo
                                      
void setup(){ //indicamos las entradas que tendra el arduino
  
  //sensores
  for(int i=0;i<n;i++){
  pinMode(asiento[i],INPUT); 
  pinMode(cinturon[i],INPUT);
  pinMode(broche[i],INPUT);
  //indicadores
  pinMode(leds[i],OUTPUT);
  pinMode(buzzer[i],OUTPUT);
  }
  
  Serial.begin(9600);
  Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms
  pinMode(EnTxPin, OUTPUT);
  digitalWrite(EnTxPin, LOW); //RS485 como receptor
}

void loop(){

  for(int i=0;i<n;i++){
  
  asientoReading = analogRead(asiento[i]);
  asientoVoltage = float(map(asientoReading,0,1023,0,5000)); 
  asientoResistance = (10000*(5000-asientoVoltage))/asientoVoltage;
  if(0<asientoResistance && asientoResistance<1125){ //1125-1800
    
     brocheReading = digitalRead(broche[i]);
     if(brocheReading==LOW ){ //Normalmente abierto y pull-up
      
      cinturonReading = analogRead(cinturon[i]);
      cinturonVoltaje = float(map(cinturonReading,0,1023,0,5000));
      cinturonResistance = 10000*(5000-cinturonVoltaje)/cinturonVoltaje;
      if (cinturonResistance <= 4000){ // 200g  de fuerza en adelante
       Estado[i]=0;//sin alarma
      }else{
       Estado[i]=1;//alarma activada
      }
   
      }else{
        Estado[i]=2;//alarma  en activacion
      }  
  }else{
    Estado[i]=3; //desocupado
  }

  if(Estado[i]==2){
    if(flag_Estado[i]==0){
      lastTime[i]=millis();
      flag_Estado[i]=1;
      alarma[i]=0;//indica ocupacion y alarma desactivada
    }else{ 
      if(millis() - lastTime[i] >= tiempo_espera){ //consulta de tiempo
        digitalWrite(leds[i],HIGH);//activar alarma visual
        digitalWrite(buzzer[i],HIGH); //activar alarma sonora  
        alarma[i]=1;} //alarma activada en un determinado tiempo
    }
  }else if(Estado[i]==0){
    digitalWrite(leds[i],LOW);//desactivar alarma visual
    digitalWrite(buzzer[i],LOW); //desactivar alarma sonora 
    alarma[i]=0; //indica ocupacion y alarma desactivada
    flag_Estado[i]=0;
  }else if(Estado[i]==3){
    digitalWrite(leds[i],LOW);//desactivar alarma visual
    digitalWrite(buzzer[i],LOW); //desactivar alarma sonora 
    alarma[i]=2;//indica asiento no ocupado
    flag_Estado[i]=0;
  }else if(Estado[i]==1){
    digitalWrite(leds[i],HIGH);//activar alarma visual
    digitalWrite(buzzer[i],HIGH); //activar alarma sonora 
    alarma[i]=1;//alarma activada instantaneamente
  }
  
  }

}

void serialEvent() {
  
    if(Serial.available())
  {
    if(Serial.read()=='I') //Si recibimos el inicio de trama
    {
        int direccion=Serial.parseInt(); //recibimos la direccion 
        if(direccion==mydireccion) //Si direccion es la nuestra
        {
                if(Serial.read()=='F') //Si el fin de trama es el correcto
                 {
                   digitalWrite(EnTxPin, HIGH); //rs485 como transmisor
                   Serial.print("i"); //inicio de trama  
                   Serial.print(mydireccion); //direccion 
                   for(int i=0;i<n;i++){
                    Serial.print(",");       
                    Serial.print(alarma[i]); //envio de cada elemnto del array alarma del sensor    
                   }
                   Serial.print("f"); //fin de trama  
                   Serial.flush(); //Esperamos hasta que se envíen los datos
                   digitalWrite(EnTxPin, LOW); //RS485 como receptor             
                 }
        }
    }
  }
  delay(10); 
}



