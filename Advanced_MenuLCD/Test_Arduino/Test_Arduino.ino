#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
#include <HX711.h>
#include <Keyboard.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Wire.h>
#define DOUT  A2
#define CLK  A1
#define exitKey '#'

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
boolean bandera = false;
boolean bandera2 = false;
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
    lcd.begin();
    readVariables();//Es recomendable leerlas para tener valores diferente de 0 en las variables
    initPLC();
}

void loop() {    
  Temperatura = 10;
  Peso = 188;
  Presion = 1; 
  exitBtnPressedOnDelay(1000);
  Temperatura = 60;
  Peso = 188;
  exitBtnPressedOnDelay(1000);
  Temperatura = 60;
  Peso = 380;
  exitBtnPressedOnDelay(1000);
  Temperatura = 60;
  Peso = 380;
  exitBtnPressedOnDelay(1000);
  Temperatura = 40;
  Peso = 330;
  exitBtnPressedOnDelay(1000);
  Temperatura = 40;
  Peso = 330;
  
}
void readVariables(){
  //Aqui se va a leer temperatura, presion y peso.
  //Temperatura 
  //sensorValue = analogRead(analogInTermocupla);
  //Temperatura = sensorValue * 0.055 + 13.75;
  //Peso
  //balanza.set_scale(630.9); // Establecemos la escala esta es el resultado de la división de la lectura de calibracion entre 771.6179 para mostrar el peso grain
  //balanza.tare(100);
  //Peso = balanza.get_units(5);
  //Presion
  //Como el sensor aun no esta envio la temperatura --> Por favor agregar aqui la lectura del sensor de presion PSI
  //Presion = Temperatura;                 
}
void readKeyboard()
{
  key = keypad.getKey();  
  if(!(key == '1' || key == '2' || key == '3')){
    publishToPLC();
  }
  switch (key){
        case '1':
        while(1){
          limpiarPantalla();
          outputValue = Temperatura;
          mensajeSalida = String(outputValue)+"'C";
          printMsg("TEMPERATURA",mensajeSalida,5,2);
          if(exitBtnPressedOnDelay(1000)){
              break;
          }  
        }         
          
        break;

        case '2':
        while(1){
          outputValue = Peso;
          mensajeSalida = String(outputValue)+" GRAMOS";
          printMsg("NIVEL",mensajeSalida,0,0);
          
          if(exitBtnPressedOnDelay(1000)){
              break;
          }
          
        }
        break;
        case '3':
        while(1){
          limpiarPantalla();
          outputValue = Presion;
          mensajeSalida = String(outputValue)+"Psi";
          printMsg("Presion",mensajeSalida,5,2);
          
          //un delay sin dejar de leer el teclado
          if(exitBtnPressedOnDelay(1000)){
              break;
          }
        }           
        break;
        default:
          InitialMsg();
          break;
        
  }
} 
void InitialMsg()
{
    printMsg("VARIABLE","1Temp 2Nvl 3Tiem",4,0);
}

void limpiarPantalla(){
  lcd.clear();
  mensajeAntiguoLinea1 = "";
  mensajeAntiguoLinea2 = "";
}
void printMsg(String mensajeLinea1,String mensajeLinea2,int x1, int x2){
  if(!(mensajeLinea1==mensajeAntiguoLinea1)||!(mensajeLinea2==mensajeAntiguoLinea2))
  {
    lcd.clear();
    lcd.setCursor(x1,0);
    lcd.print(mensajeLinea1);
    lcd.setCursor(x2,1);
    lcd.print(mensajeLinea2);
    mensajeAntiguoLinea1 = mensajeLinea1;
    mensajeAntiguoLinea2 = mensajeLinea2;
  }
}
boolean exitBtnPressedOnDelay(int timeDelay){
  boolean returned = false;
  //un delay sin dejar de leer el teclado
    startMillis = millis();
    endMillis = millis()+timeDelay;
      //Aprovechando el Delay se leen las variables y se publica al plc el estado
    readVariables();
    while(millis()<=endMillis){
      key = keypad.getKey();
      publishToPLC();    
      if(key == exitKey){
        returned = true;
        break;
      }
    }
  return returned;              
}

