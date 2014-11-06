#include <Wire.h>
#include "RTClib.h" // Importacion de librerias


// Declaracion de variables

 RTC_DS1307 RTC;
 int pindatos = 2;
 int clockpin = 3;
 int latchpin = 4;
 int pindatos2 = 8;
 int clockpin2 = 9;
 int latchpin2= 10;
 byte data1 = 0;
 byte data2 = 0;
 //Varaibles contadoras del estado que se encuentra el boton
 const int  buttonPin = 11;    
 int buttonPushCounter = 1;  
 int buttonState = 0;    
 int buttonState1 = 0;
 int lastButtonState = 0;
 int bandera = 0;  // Valor que mantiene el boton que va ser utilizado para acceder a las diferentes funciones del reloj
 const int buttonPin2 = 7;
 // Variables donde se van a cargar los datos de los minutos, segundos y horas 
 int minus;
 int secus;
 int hor;
 
void setup(){
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();        
  if (! RTC.isrunning()) {
  Serial.println("El Real Time Clock no se encuentra trabajando!");} // Notifica si el RTC no se encuentra trabajando
 
  pinMode(pindatos, OUTPUT);
  pinMode(clockpin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(pindatos2, OUTPUT);
  pinMode(clockpin2, OUTPUT);
  pinMode(latchpin2, OUTPUT); 
  pinMode(buttonPin2, INPUT);
}

void loop() {
  
  buttonState = digitalRead(buttonPin); // Se utiliza para el estado del boton
  buttonState1 = digitalRead(buttonPin2);//Se utiliza para el estado del sensor de proximidad
  
  Serial.print(bandera); //Se utiliza para controlar el estado de la bandera 
                        //y que el programador sepa en que estado se encuentra
  
  if(lastButtonState != buttonState){
    if ((buttonState == HIGH) ||(buttonState1 == HIGH)) { //Determina si hay un cambio en el estado del boton o del sensor. 
    bandera++;
    if(bandera>2){   //Regresa el valor de la bandera a 0 si excede la cantidad de igual o mayor a 3
      bandera=0;
    }
    Serial.println(bandera);
    Serial.println(buttonState1);
    delay(300);}} // se realiza esta pausa de tiempo para que el cambio de estado del boton sea mas exacto.
  
  //Si el estado de la bandera es 0, se estara ejecutando el reloj
  if(bandera == 0){
    reloj();
    Serial.println("Modo Reloj");
  }
  //Si el estado de la bandera cambia a 1, se ejecutara la funcion de termometro
  if(bandera == 1){
    temperatura();
    Serial.println("Modo temperatura");
  }
  // Si el estado es igual a 2 se convierte en temperatura
  if(bandera == 2){
    Serial.println("Modo cronometro");
     hor=0;
     minus=0;
     secus=0;
     while (bandera == 2){
     cronometro();
     }
  }
  
} 
 
//Funciones del shiftregister 
     
void writeByte(byte data, byte set)
{
  int d,c,l;
  if (set == 1)
  {
    d = 2;
    c = 3;
    l = 4;
  }
  else if (set == 2)
  {
    d = 8;
    c = 9;
    l = 10;
  }
  shiftOut(d, c, MSBFIRST, data);
  digitalWrite(l, HIGH);
  digitalWrite(l, LOW);
}

void reloj(){
  Serial.begin(57600);
  DateTime now = RTC.now(); // Carga la informacion del tiempo
  int mins = now.minute();
  int secs = now.second();
  int hr = now.hour();
  
  //Este if determina si la hora es mayor a 12, para restarle 12 y de esa manera mantener el reloj en formato de 12hrs
  if (hr>12)
  {
    hr = hr-12;
  }
  byte data1 = 0;
  byte data2 = 0;

  for (int i = 0; i < 4 ;i ++)
  {
    if (bitRead(hr,i) == 1)
    {
      bitWrite(data1, 3-i, 1);
      Serial.print(hr);
    }
  }
 
  for (int i = 2;i < 6;i ++)
  {
    if (bitRead(mins,i) == 1)
    {
      bitWrite(data1,9-i, 1);
    }
  }

  for (int i =0;i<2;i++)
  {
    if (bitRead(mins,i)==1)
    {
      bitWrite(data2,1-i,1);
    }
  }


  for (int i =2;i<8;i++)
  {
    if (bitRead(secs,i-2)==1)
    {
      bitWrite(data2,9-i,1);
    }
  }
  
  writeByte(data1,1);
  writeByte(data2,2);
  delay(1000);

}


//Funcion que realiza el cronometro
void cronometro(){

  byte data1 = 0;
  byte data2 = 0;

  for (int i = 0;i < 4;i ++)
  {
    if (bitRead(hor,i) == 1)
    {
      bitWrite(data1,3-i,1);
     
    }
  }
 
  for (int i = 2;i < 6; i ++)
  {
    if (bitRead(minus,i) == 1)
    {
      bitWrite(data1,9-i, 1);
    }
  }


  for (int i = 0;i < 2;i ++)
  {
    if (bitRead(minus,i) == 1)
    {
      bitWrite(data2,1-i,1);
    }
  }
 
  for (int i =2;i<8;i++)
  {
    if (bitRead(secus,i-2)==1)
    {
      bitWrite(data2,9-i,1);
    }
  }

  writeByte(data1,1);
  writeByte(data2,2);

  delay(1000);
  
  if (secus > 58){
    minus = minus + 1;
    secus = 0;
  }
  if (minus > 58){
    hor = hor + 1;
    minus=0;
  }
  if (hor > 12){
    hor = 0;
  }   
  secus = secus + 1;
}

void temperatura(){
  delay(1000);
  const int temperaturePin = 0;
  Serial.begin(9600);
  float voltage;
  int degreesC;
  int dato1 = 0;
  int dato2 = 0;
  voltage = getVoltage(temperaturePin);
  degreesC = (voltage - 0.5) * 100.0;

  for (int i = 2;i < 6;i ++)
  {
    if (bitRead(degreesC,i) == 1)
    {
      bitWrite(dato1, 9-i, 1);
    }
  }
 
  for (int i =0;i<2;i++)
  {
    if (bitRead(degreesC,i) == 1)
   
      bitWrite(dato2,1-i,1);
    }
  writeByte(dato1,1);
  writeByte(dato2,2);
  
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(degreesC);
  delay(1000);
}

float getVoltage(int pin)
{
  return (analogRead(pin) * 0.004882814); 
}
