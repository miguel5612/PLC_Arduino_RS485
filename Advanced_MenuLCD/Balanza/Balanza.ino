#include "HX711.h"
#include <SoftwareSerial.h>

#define DOUT  A2
#define CLK  A1
#define longBytesReceive 10

int peso = 0;
String strPeso;
char charPeso[10] ;
HX711 balanza(DOUT, CLK);
SoftwareSerial ArduinoMega(10, 11); // RX, TX

void setup() {
  ArduinoMega.begin(9600);
  Serial.begin(9600);
  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(-982.4089947); // Establecemos la escala
  balanza.tare(20);  //El peso actual es considerado Tara.
  Serial.println("Listo para pesar");  
}

void loop() {
  //peso = balanza.get_units(20);
  peso = random(0,3000);
  strPeso = String(peso);
  strPeso.toCharArray(charPeso,longBytesReceive);
  ArduinoMega.write(charPeso,longBytesReceive);
  Serial.println(String(charPeso));
  Serial.print("Peso: ");
  Serial.print(peso);
  Serial.println(" kg");
  delay(500);
}
 


