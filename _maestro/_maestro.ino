#include <LiquidCrystal_I2C.h>
#include <LedControl.h>

//pin control conexion serial
const int EnTxPin =  2; // HIGH:TX y LOW:RX

//asientos por arduino
const int n=2;
//datos leidos
int dato[n];
//numero de arduinos eslavos a consultar
const int num_ardu=1;
// direciones de arduinos esclavos orden asiento 1, 2, 3, ..., n
const int direccion_ard[num_ardu]={101};

//matriz de leds 6x8 = 48 leds
//pin 51 is connected to the DataIn     
//pin 52 is connected to the CLK 
//pin 53 is connected to LOAD 
//We have only a single MAX72XX.
LedControl lc=LedControl(51,52,53,1);

 int acumulador_alarma;
 int acumulador_ocupacion;
 int alarma[48]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
//max de asientos 48

LiquidCrystal_I2C lcd=LiquidCrystal_I2C(0x27,16,2);

void setup()                          
   {   
      lcd.init();
      lcd.backlight();
      
      //conexion serial
      Serial.begin(9600);
      Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms
      pinMode(EnTxPin, OUTPUT);
      digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor

   /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,4);
  /* and clear the display */
  lc.clearDisplay(0);
  
   }

void loop()
   {
  acumulador_alarma=0;
  acumulador_ocupacion=n*num_ardu;

  for(int i=0;i<num_ardu;i++){
  //---solicitamos una lectura del sensor----------
  Serial.print("I"); //inicio de trama
  Serial.print(direccion_ard[i]);//direccion del esclavo (probar en tinkercad direccion con numero entero)
  Serial.print("F"); //fin de trama
  Serial.flush();    //Esperamos hasta que se envíen los datos
  //----Leemos la respuesta del Esclavo-----
  digitalWrite(EnTxPin, LOW); //RS485 como receptor
  if(Serial.find("i")) //esperamos el inicio de trama
  {
      int esclavo=Serial.parseInt();  //recibimos la direccion del esclavo
      for(int j=0;j<n;j++){
        dato[j]=Serial.parseInt();  //recibimos el dato
      }
      
      if(Serial.read()=='f' && esclavo==direccion_ard[i]) //si fin de trama y direccion son los correctos
      {
       for(int j=0;j<n;j++){
        alarma[j+n*i]=dato[j];  //se almacena en la matriz
       }
      }
  }
      digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor
  //----------fin de la respuesta----------
  delay(100);
  }
 
    for(int i=0; i<n*num_ardu;i++){
      int val=alarma[i];             //val==1  alarma ON
                                     //val==0  alarma OFF
      if (val==1)                    //val==2  asiento no ocupado
      acumulador_alarma++;

      if (val==2)
      acumulador_ocupacion--;
    }
    
    lcd.setCursor(0,0);
    lcd.print("OCUPADOS:");
    lcd.setCursor(10,0);
    lcd.print(acumulador_ocupacion);
    lcd.setCursor(0,1);
    lcd.print("ALARMA:");
    lcd.setCursor(10,1);
    lcd.print(acumulador_alarma);

    Matriz_leds();
    delay(1000);
   }
void Matriz_leds()
   {
    int i=0;
    for(int row=0;row<6;row++) { 
    for(int col=0;col<8;col++) { 
      if(alarma[i]==1){
      lc.setLed(0,row,col,true);
      }else{
      lc.setLed(0,row,col,false);
      }
      i++;
    }}
  }
   
