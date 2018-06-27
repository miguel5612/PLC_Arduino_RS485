// Proceso de dosificacion
// Fecha de creacion: 24.06.2018

//libreria para controlar las comunicaciones SPi
#include <Wire.h>
//Librerias empleadas para modbus
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
//Libreria empleada para la balanza
#include "HX711.h"
//Libreria para controlar el teclado
#include <Keypad.h>
//Libreria para controlar la pantalla
#include <LiquidCrystal_I2C.h>


//Pines de la balanza
#define DOUT  A2
#define CLK  A1
#define exitKey '#'
#define longBytesReceive 10
#define minMilistoPrintLCD 200

//Configuracion de la pantalla, termocupla y teclado
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const byte rowsCount = 4;
const byte columsCount = 4;
const byte rowPins[rowsCount] = {A15, A14, A13, A12}; //connect to the row pinouts of the keypad
const byte columnPins[columsCount]= {A11, A10, A9, A8};
const int analogInTermocupla = 0;  // Analog input pin that the potentiometer is attached to
const int analogInCelda = 2;//Entrada de la celda de carga 

//PLC Settings

#define RS485TxEnablePin 2
#define RS485Baud 9600
#define RS485Format SERIAL_8E1

//Arduino Variables


int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int Peso,Temperatura,Presion;
long startMillis = 0;
long endMillis = 0;
String mensajeAntiguoLinea1 = "";
String mensajeAntiguoLinea2 = "";
String mensajeSalida = "";
char key;
char tempKey ;
bool bandera = true;
char inChar;
char inData[longBytesReceive];
int index;
long oldMilis;

// definen los símbolos en los botones de los teclados 
char keys[rowsCount][columsCount] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '*','0','#', 'D' }
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);
HX711 balanza(DOUT, CLK);
LiquidCrystal_I2C lcd(0x27, 16, 2);
modbusDevice regBank;
modbusSlave slave;



//Codigo de inicializacion de la balanza
void initBalanza()
{
  balanza.set_scale(-982.4089947); // Establecemos la escala
  balanza.tare(20);  //El peso actual es considerado Tara.
  
}


//Cambiar de acuerdo a las necesidades.
//El plc programado es un DVP-14SS2.
void publishToPLC(){
  regBank.set(40001, (word) abs(Temperatura));
  regBank.set(40002, (word) abs(Presion));
  regBank.set(40003, (word) abs(Peso));
  regBank.set(40004, (word) 5);
  slave.run();
}
void initPLC(){
  //Assign the modbus device ID.  
  regBank.setId(1);
  regBank.add(30001);  
  regBank.add(30002);  
  regBank.add(40001);  
  regBank.add(40002);
  regBank.add(40003);  
  regBank.add(40004);
  slave._device = &regBank;  
  slave.setBaud(&Serial,RS485Baud,RS485Format,RS485TxEnablePin);   
}

void setup() 
{
    initBalanza();
    lcd.begin();
    Serial2.begin(9600);
    readVariables();//Es recomendable leerlas para tener valores diferente de 0 en las variables
    initPLC();
}

void loop() {    
    readKeyboard(); //Lee el teclado, sino se comunica con el PLC, sino publica el mensaje en la pantalla.
    if(!validateKey(key)){
      InitialMsg();
    }
    readVariables();
    publishToPLC();
 }
void readVariables(){
  //Aqui se va a leer temperatura, presion y peso.
  //Temperatura 
  sensorValue = analogRead(analogInTermocupla);
  Temperatura = sensorValue * 0.055 + 13.75;
  //Peso
  index = 0;
  if(Serial2.available()){
    Serial2.readBytes(inData,longBytesReceive);   
  }
  Peso = String(inData).toInt();
  //Peso = balanza.get_units(20);
  //Presion
  //Como el sensor aun no esta envio la temperatura --> Por favor agregar aqui la lectura del sensor de presion PSI
  Presion = Temperatura;                 
}
void readKeyboard()
{
  tempKey = keypad.getKey();
  if(validateKey(tempKey) == 1){
    key = tempKey;
  }else if(validateExitKey(tempKey) == 1){
    key = ' '; //Tecla que no hace ninguna accion en el programa
  }else 
  {
  switch (key){
  
        case '1':
          outputValue = Temperatura;
          mensajeSalida = String(outputValue)+"'C";
          printMsg("TEMPERATURA",mensajeSalida,5,2);  
        break;

        case '2':
          outputValue = Peso;
          mensajeSalida = String(outputValue)+" GRAMOS";
          printMsg("NIVEL",mensajeSalida,0,0);
        break;
        
        case '3':
          outputValue = Presion;
          mensajeSalida = String(outputValue)+"Psi";
          printMsg("Presion",mensajeSalida,5,2);
        break;        
    }
  }
} 
void InitialMsg()
{
    printMsg("VARIABLE","1Temp 2Nvl 3Pres",4,0);
}

void limpiarPantalla(){
  lcd.clear();
  mensajeAntiguoLinea1 = "";
  mensajeAntiguoLinea2 = "";
}
void printMsg(String mensajeLinea1,String mensajeLinea2,int x1, int x2){
  if(!(mensajeLinea1==mensajeAntiguoLinea1)||!(mensajeLinea2==mensajeAntiguoLinea2) && millis()>oldMilis+minMilistoPrintLCD)
  {
    oldMilis = millis();
    lcd.clear();
    lcd.setCursor(x1,0);
    lcd.print(mensajeLinea1);
    lcd.setCursor(x2,1);
    lcd.print(mensajeLinea2);
    mensajeAntiguoLinea1 = mensajeLinea1;
    mensajeAntiguoLinea2 = mensajeLinea2;
  }
}
bool validateKey(char keyPressed)
{
  if(keyPressed=='1' || keyPressed == '2' || keyPressed == '3')
  {
    return true; //Valid key
  }
  return false;
}
bool validateExitKey(char keyPressed)
{
  if(keyPressed == exitKey)
  {
    return true; //Valid key
  }
  return false;
}

