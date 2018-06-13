#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
modbusDevice regBank;
modbusSlave slave;

#define RS485TxEnablePin 2
#define RS485Baud 9600
#define RS485Format SERIAL_8E1


#define LED1 11
#define LED2 12

void setup()
{   
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
pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT); 
}

void loop()
{
  regBank.set(40001, (word) 8);
  regBank.set(40002, (word) 7);
  regBank.set(40003, (word) 6);
  regBank.set(40004, (word) 5);
  slave.run();  
}
