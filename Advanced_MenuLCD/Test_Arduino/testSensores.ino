#include <Wire.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include "HX711.h"
#define CLK  1
#define DOUT 2
HX711 balanza(DOUT, CLK);


// These constants won't change. They're used to give names to the pins used:
const int analogInTermocupla = 0;  // Analog input pin that the potentiometer is attached to
const int analogInCelda = 2;//Entrada de la celda de carga 
const int digitalOutResistor = 9; // Encendido de resistencia Termica
const int digitalOutPiloto = 8; // Encendido Luz Piloto
const int digitalOutElectroval = 10; // Apertura Electrovalvula


int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)



int umbralTempInf = 35 ;            // Temparatura de encender resistencia termica.
int umbralTempSup = 60;              //Temperatura para apagar la resistencia  termica.
int setpointTemp = 70;            // Temperatura de encender luz piloto

int umbralNivelInf =188;             //Apertura de electrovalvula 
int umbralNivelSup=300;             // Envase Lleno, cerrar electrovalvula 
int peso;
//const int electrovalvula=10; // Control Electrovalvula




const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte rowsCount = 4;
const byte columsCount = 4;
// definen los s�mbolos en los botones de los teclados 
char keys[rowsCount][columsCount] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '#','0','*', 'D' }
};
const byte rowPins[rowsCount] = {A15, A14, A13, A12}; //connect to the row pinouts of the keypad
const byte columnPins[columsCount]= {A11, A10, A9, A8};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);

void setup() {
    lcd.begin(16, 2);
    Serial.begin(9600);
    }

void loop() {
    char key = keypad.getKey();

    lcd.clear (); 
    lcd.setCursor (4,0); 
    lcd.print ("VARIABLE"); 
    lcd.setCursor (0,2); 
    lcd.print ("1 Temp"); 
    lcd.setCursor (6,2); 
    lcd.print ("2 Nive"); 
    lcd.setCursor (11,2); 
    lcd.print ("3 Tiem"); 
    
    switch (key){
      
        case '1':
        while(1){
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("TEMPERATURA");
          sensorValue = analogRead(analogInTermocupla);
          outputValue = sensorValue * 0.055 + 13.75;
          lcd.setCursor(5,1);
          lcd.print(outputValue);
          lcd.setCursor(8, 2);
          lcd.print("'C");
          key = keypad.getKey();
          if(key == '#'){
            break;
          }
          delay(100);
          }
          
        break;

        case '2':
        while(1){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("NIVEL");
          balanza.set_scale(630.9); // Establecemos la escala esta es el resultado de la divisi�n de la lectura de calibracion entre 771.6179 para mostrar el peso grain
          balanza.tare(100);
          lcd.setCursor(0, 2);
          peso = balanza.get_units(5);
          lcd.print(peso);
          lcd.setCursor(8, 2);
          lcd.print("GRAMOS");
          delay(100);
          if(key == '#'){
            break;
          }
          delay(100);
        }
        break;

        default:
          lcd.clear (); 
          lcd.setCursor (4,0); 
          lcd.print ("VARIABLE"); 
          lcd.setCursor (0,2); 
          lcd.print ("1 Temp"); 
          lcd.setCursor (6,2); 
          lcd.print ("2 Nive"); 
          lcd.setCursor (11,2); 
          lcd.print ("3 Tiem"); 
          delay(100);
          break;
        

    }
   
  
     
//  if (outputValue >= umbral && outputValue <= umbral2)
//    digitalWrite(digitalOutResistor, HIGH);
//  else
//    digitalWrite(digitalOutResistor, LOW);
//
//  if (outputValue >= peligro)
//    digitalWrite(digitalOutPiloto, HIGH);
//  else
//    digitalWrite(digitalOutPiloto, LOW);
   
  //if (balanza.get_units(5)== llenado && balanza.get_units(5)==completo)
    //digitalWrite(control, HIGH);
  //else
   // digitalWrite(control, LOW);



  //delay(200);
   //lcd.setCursor(1, 1);
  //lcd.print(outputValue, 1);
 // lcd.setCursor(6, 1);
  //lcd.print("centigrade");
}
 
