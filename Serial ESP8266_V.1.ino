#include <ESP8266WiFi.h>
//#include <MicroGear.h>
#include <SoftwareSerial.h>

SoftwareSerial NodeMCU_SoftSerial(D1,D2);
char c;
 String dataIn;
void setup() {

Serial.begin(57600);
NodeMCU_SoftSerial.begin(9600);
}

void loop() {
  while(NodeMCU_SoftSerial.available()>0)
  {
    c = NodeMCU_SoftSerial.read();
    if (c=='s'){
    break;
    } else {
    dataIn = dataIn+c;
    }
    
      }
      Serial.print(dataIn);
      c = 0;
      dataIn = "";
} 
