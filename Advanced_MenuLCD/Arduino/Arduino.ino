#include <HX711.h>
#include <Keyboard.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Wire.h>


#define DOUT  A2
#define CLK  A1
#define longDelay 1000
#define exitKey '#'

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const byte rowsCount = 4;
const byte columsCount = 4;
const byte rowPins[rowsCount] = {A15, A14, A13, A12}; //connect to the row pinouts of the keypad
const byte columnPins[columsCount]= {A11, A10, A9, A8};
const int analogInTermocupla = 0;  // Analog input pin that the potentiometer is attached to
const int analogInCelda = 2;//Entrada de la celda de carga 

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int peso;
long startMillis = 0;
long endMillis = 0;
String mensajeAntiguoLinea1 = "";
String mensajeAntiguoLinea2 = "";
String mensajeSalida = "";

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


void setup() 
{
    lcd.begin();
    Serial.begin(9600);
}

void loop() {
    InitialMsg();
    readKeyboard();
}

void readKeyboard()
{
  char key = keypad.getKey();  
  Serial.println(key);
  switch (key){
        case '1':
        while(1){
          limpiarPantalla();
          sensorValue = analogRead(analogInTermocupla);
          outputValue = sensorValue * 0.055 + 13.75;
          mensajeSalida = String(outputValue)+"'C";
          printMsg("TEMPERATURA",mensajeSalida,5,2);
        
          if(exitBtnPressedOnDelay()){
              break;
          }  
        }         
          
        break;

        case '2':
        while(1){
          balanza.set_scale(630.9); // Establecemos la escala esta es el resultado de la división de la lectura de calibracion entre 771.6179 para mostrar el peso grain
          balanza.tare(100);
          peso = balanza.get_units(5);
          mensajeSalida = String(peso)+" GRAMOS";
          printMsg("NIVEL",mensajeSalida,0,0);
          
          if(exitBtnPressedOnDelay()){
              break;
          }
          
        }
        break;
        case '3':
        while(1){
          limpiarPantalla();
          sensorValue = analogRead(analogInTermocupla);
          outputValue = sensorValue * 0.055 + 13.75;
          mensajeSalida = String(outputValue)+"Psi";
          printMsg("Presion",mensajeSalida,5,2);
          
          //un delay sin dejar de leer el teclado
          startMillis = millis();
          endMillis = millis()+longDelay;
          if(exitBtnPressedOnDelay()){
              break;
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
boolean exitBtnPressedOnDelay(){
  boolean returned = false;
  //un delay sin dejar de leer el teclado
    startMillis = millis();
    endMillis = millis()+longDelay;
    while(millis()<=endMillis){
      key = keypad.getKey();
      if(key == exitKey){
        returned = true;
        break;
      }
    }
  return returned;              
}
