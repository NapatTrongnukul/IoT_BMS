#include<EEPROM.h> 
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <UTFTGLUE.h> 
UTFTGLUE myGLCD(0,A2,A1,A3,A4,A0);

#define Button 20
SoftwareSerial espSerial(18, 19);

int SWState = 0;
int LastSWState = 0;

//double solar_jan[] = {0,150,250,300,350,250,250,0,170,260,0,55,36,72,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;
float solar_jan[] = {0,150,250,300,350,250,250,155,170,260,33,55,36,72,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ;

const float batt_voltVSsoc[21][2] = {{9.7,0},{10.15,5},{10.4,10},{10.5,15},{10.7,20},{10.95,25},{11.2,30},{11.3,35},{11.5,40},{11.6,45},{11.7,50},{11.8,55},{12,60},{12.2,65},{12.3,70},{12.4,75},{12.5,80},{12.55,85},{12.6,90},{12.7,95},{12.75,100}};   

float batt_curr[sizeof(solar_jan)/sizeof(int)];
float batt_volt[sizeof(solar_jan)/sizeof(int)];
float soc[sizeof(solar_jan)/sizeof(int)];
float dod[sizeof(solar_jan)/sizeof(int)];
float batt_temp[sizeof(solar_jan)/sizeof(int)];
int charge_flag[sizeof(solar_jan)/sizeof(int)];
int discharge_flag[sizeof(solar_jan)/sizeof(int)];
int error_flag[sizeof(solar_jan)/sizeof(int)];

float batt_cap[sizeof(solar_jan)/sizeof(int)];
String error_message[sizeof(solar_jan)/sizeof(int)];


float led_power = 120;
float batt_curr_lim = 36;
float t_sampling = 60;

int test = 1;

int round5(int n) {
  return (n/5 + (n%5>2)) * 5;
}

void init_bms(){
   batt_cap[0] = 43200;
   batt_volt[0] = 12.3;
   soc[0] = 0.7;
   dod[0] = 1 - soc[0];
   batt_temp[0] = 35;
   
   }

void operation_Mode(int count) {
     if (solar_jan[count] > 10 && soc[count] <=1 && batt_temp[count] <=100) {
     charge_flag[count]= 1;
     discharge_flag[count] = 0;
     error_flag[count] = 0;
   } else if (solar_jan[count] <= 10 && dod[count] <=0.5 && batt_temp[count] <=100){
     charge_flag[count] = 0;
     discharge_flag[count] = 1;
     error_flag[count] = 0;
   } else if (solar_jan[count] <= 10 && dod[count] >0.5 && batt_temp[count] <=100){
     charge_flag[count] = 0;
     discharge_flag[count] = 0;
     error_flag[count] = 1;
   } else if (solar_jan[count] > 10 && soc[count] >1 && batt_temp[count] <=100){
     charge_flag[count] = 0;
     discharge_flag[count] = 0;
     error_flag[count] = 2;
   } else if (batt_temp[count]>100){
     charge_flag[count] = 0;
     discharge_flag[count] = 0;
     error_flag[count] = 3;
   }else{
     charge_flag[count] = 0;
     discharge_flag[count] = 0;
     error_flag[count] = 4;
   }
     }


void errorCheck(int count){
  int errorflag = error_flag[count];
  switch(errorflag) {
  case 0:
    error_message[count] = "Normal Operation";
    Serial.println("Normal Operation");
    break;
  case 1:
    error_message[count] = "DOD too high";
    Serial.println("DOD too high");
    delay(100);
    break;
  case 2:
    error_message[count] = "SOC too high";
    Serial.println("SOC too high");
    delay(100);
    break;
  case 3:
    error_message[count] = "Batt Temp too high";
    Serial.println("Batt Temp too high");
    delay(100);
    break;
  case 4:
    error_message[count] = "Unidentified Error";
    Serial.println("Unidentified Error");
    delay(100);
    break;
  case 5:
    error_message[count] = "Invalid Look-up Table";
    Serial.println("Invalid Look-up Table");
    delay(100);
    break;
  }
}

void battCurrCal(int count) {
    if (charge_flag[count]+discharge_flag[count]!=0){
      if (charge_flag[count] == 1 && discharge_flag[count]==0){
        batt_curr[count] = solar_jan[count]/batt_volt[count];
          if (batt_curr[count]>batt_curr_lim){
      batt_curr[count] = batt_curr_lim;
      return;
    }
   } else if (charge_flag[count] == 0 && discharge_flag[count]==1){
        batt_curr[count] = (-1)*led_power/batt_volt[count];
        if(batt_curr[count]<-1*batt_curr_lim){
          batt_curr[count] = -1*batt_curr_lim;
        return;
        }
   }
 } else {
    errorCheck(count);
   }
 }

 void battTempCal(int count) {
    batt_temp[count+1] = batt_temp[count]+0.0215*(abs(batt_curr[count]));
    return;
 }
 
 void socCal(int count) {
  
    soc[count+1] = soc[count]+(1/batt_cap[0])*(batt_curr[count])*t_sampling;
    int soc_batt_int = round(soc[count]*100);
    int soc_batt_int_t = round(soc[count+1]*100);
    
    soc_batt_int = round5(soc_batt_int);
    soc_batt_int_t = round5(soc_batt_int_t);
   
    for (int i =0;i<(sizeof(batt_voltVSsoc)/sizeof(int)/4);i++){
     int x1 = soc_batt_int-batt_voltVSsoc[i][1];
     int x2 = soc_batt_int_t-batt_voltVSsoc[i][1];
     if (x1 ==0){
      batt_volt[count] = batt_voltVSsoc[i][2];
     }
     if (x2 ==0){
      batt_volt[count+1] = batt_voltVSsoc[i][2];
     }
     
    }
    dod[count] = 1-soc[count];
    dod[count+1] = 1-soc[count+1];
  }

 void updateLED(int count){

    if (error_flag[count]==0 &&charge_flag[count] ==1 && discharge_flag[count]==0){
    digitalWrite(41,LOW); //ctrl yellow 
    digitalWrite(51,LOW); //ctrl red
    digitalWrite(45,HIGH); //ctrl green
    } else if(error_flag[count]==0 &&charge_flag[count] ==0 && discharge_flag[count]==1){
    digitalWrite(41,HIGH); //ctrl yellow 
    digitalWrite(51,LOW); //ctrl red
    digitalWrite(45,LOW); //ctrl green
    } else if(error_flag[count]!=0){
    digitalWrite(41,LOW); //ctrl yellow 
    digitalWrite(51,HIGH); //ctrl red
    digitalWrite(45,LOW); //ctrl green
    } else {
    digitalWrite(41,LOW); //ctrl yellow 
    digitalWrite(51,HIGH); //ctrl red
    digitalWrite(45,LOW); //ctrl green
    }
    return;
 }
 

 void battISR(int count)
{
  error_message[count] = "Operation Teminated by User's Request, Contactor is Opened";
  espSerial.println(error_message[count]);
  espSerial.println("");
  myGLCD.print(error_message[count], LEFT, 250);
  delay(300);
  digitalWrite(51,HIGH);
  digitalWrite(41,LOW);
  digitalWrite(45,LOW);
  exit(0);
 }

void updateLCD(int count){
  
  myGLCD.print(String(batt_curr[count]),300, 10);
  myGLCD.print(String(batt_volt[count]), 300, 40);
  myGLCD.print(String(solar_jan[count]), 210, 70);
  myGLCD.print(String(soc[count]*100), 300, 100);
  myGLCD.print(String(dod[count]*100), 340, 130);
  myGLCD.print(String(100), 300, 160);
  myGLCD.print(String(batt_temp[count]), 340, 190);
  myGLCD.print(error_message[count], LEFT, 250);
  
  delay(100);
  myGLCD.setColor(255,255,255); 
  return;
}

void updateESP8266(int count){
  espSerial.println("Battery Current = "+String(batt_curr[count])+" A");
  espSerial.println("Battery Voltage = "+String(batt_volt[count])+" V");
  espSerial.println("PV Power = "+String(solar_jan[count])+" W");
  espSerial.println("State of Charge = "+String(soc[count]*100)+" %");
  espSerial.println("Depth of Discharge = "+String(dod[count]*100)+" %");
  espSerial.println("State of Health = "+String(100)+" %");
  espSerial.println("Battery Temperature = "+String(batt_temp[count])+" C");
  espSerial.println("Battery Status: ");
  espSerial.println(error_message[count]);
  espSerial.println("");
  delay(100);
  return;
}
 
void setup() {
  while (!Serial); 
     
  pinMode(53,OUTPUT);
  digitalWrite(53,HIGH); ///Vcc red
  pinMode(49,OUTPUT);
  digitalWrite(49,HIGH); // Vcc button
  pinMode(43,OUTPUT);
  digitalWrite(43,HIGH); // Vcc yellow
 
  pinMode(47,OUTPUT);
  digitalWrite(47,HIGH); // Vcc green

  
  pinMode(Button, INPUT);
  digitalWrite(Button,HIGH);
  attachInterrupt(digitalPinToInterrupt(Button), battISR,LOW); 
  
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  Serial.begin(9600);
  espSerial.begin(115200);
     
}

void stopOperationISR(int count) {
  if (error_flag[count]!=0){
    exit(0);
  }else{
    return;
  }
 }

void loop() {

   
  myGLCD.print("Battery Current = ", LEFT, 10);
  myGLCD.print("A", 390, 10);

  myGLCD.print("Battery Voltage = ", LEFT, 40);
  myGLCD.print("V", 390, 40);

  myGLCD.print("PV Power = ", LEFT, 70);
  myGLCD.print("W", 320, 70);

  myGLCD.print("State of Charge = ", LEFT, 100);
  myGLCD.print("%", 390, 100);

  myGLCD.print("Depth of Discharge = ", LEFT, 130);
  myGLCD.print("%", 430, 130);

  myGLCD.print("State of Health = ", LEFT, 160);
  myGLCD.print("%", 370, 160);

  myGLCD.print("Battery Temperature = ", LEFT, 190);
  myGLCD.print("C", 440, 190);

  myGLCD.print("Battery Status: ", LEFT, 220); 
 
  init_bms();
  
  for(int i = 0;i<sizeof(solar_jan)/sizeof(int);i++)
   { 
     operation_Mode(i);
     errorCheck(i);
     battCurrCal(i);
     battTempCal(i);
     socCal(i);
     updateLED(i);
     updateLCD(i);
     updateESP8266(i);
      
     
//     Serial.println("Error Flag = "+(String)error_flag[i]);
//     Serial.println("charge Flag = "+(String)charge_flag[i]);
//     Serial.println("Discharge Flag = "+(String)discharge_flag[i]);
//     Serial.println("Batt curr = "+(String)batt_curr[i]);
//     Serial.println("SOC = "+(String)soc[i]);
//     Serial.println("Batt Volt = "+(String)batt_volt[i]);
//     Serial.println("");
     stopOperationISR(i);
     delay(1000);    
  }
}
