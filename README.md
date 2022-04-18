# IoT_BMS Update

SW Section 1: Data Acquisition and Operation Mode Selection (SW-1.1 SW1.2 SW2.1)

Objective of this section: To create solar power data arrays and load them into the EEPROM of Arduino Atmega2560, which simulates the data obtained from current and voltage sensor of Solar PV and battery.

Week 1: Literature Review and Data Sourcing
What I've done: Searched for some literature regarding the solar power profile and battery voltage VS SoC and used interpolation to make look-up table for each data
Result: Arrays contain solar power generation profile and battery voltage VS SoC profile.

Week 2: Feb 28- Mar 7
 - SW-1.1 ReadPVParam --> Read PV generation profile CSV file and send it to Arduino 
   What I've done: Created functions on arduino to read Solar PV profile and calculate proper charging/discharging current.
   Results: Two functions on arduino functions "chargeCurrCal" and "dischargeCurrCal"
   Problem: Can't load the array data into the EEPROM. But finally solved it by using the function of PROGMEM

Week 3: Mar 7 - Mar 14
- SW-1.2 ReadLAMPParam --> Read Lighting load profile from CSV file and send it to Arduino software
  What I've done: Created functions on arduino to determine and update battery status.
  Results: Two functions on arduino functions " updateBattStatus" and "dischargeCurrCal"
  Problem: When the battery status is out of operation range, the software does not tell anything to the users regarding the status, so I added a so called  "status   flags" as a return parameter of this function to tell users about battery's status and to stop operation in the case of unusual operation. 
    
Week 4: Mar 21 - Mar 28
- SW-2.1 Function: OperateMode()--> Determine the operation mode of the battery according to the solar PV generation and battery status
  What I've done: Created two functions on arduino to determine the operation mode of battery and to make an look-up table of battery voltage VS SoC, to determine the battery voltage with respective to a certain value of SoC.
  Results: two function of "updateBattStatus" and "BattSOClookup"
  Problem: It is very hard to compare float value of battery SoC with the integer values in the lookup table, if I use for loop to determine the value on by one, it take a lot of time to finish. So the best decision is to use "round" function to converter float to integer so I can compare the calculated SoC with the valued in look-up table. 
  
Week 5: Mar 28- April 4 
- SW-2.2 Function BattCurrCal() --> Determine the charging current of the battery according to the PV generation
- SW-2.3 Function BattParamCal()--> Determine SOC, DOD, SOH, Temperature, Battery Charging current, Battery Discharging current
What I've done: Created two functions on arduino to determine the charging current and SOC, DOD and SOH of battery 
Problem: no problem since the process of these two functions are very similar to the function of "updateBattStatus" so the problem that I found is the same as I found 
previously.

Week 6: April 4- April 11 
- SW-3.1 Function UpdateDisplay() --> Update all the calculated values into the LCD screen
What I've done: Created a function to display charging current aSOC, DOD, SOH Temperature of battery on LCD screen via SPI communication
Problem: 
1) There are a lot of conflicts in LCD screen library because different vendors created different library files and command function. So, it took me quite some time to find the library that matches the LDC I brought and make it works.
2) To update the LCD screen every 2 second, I found that it is more effective to put the "myGLCD.print" function in the void loop than creating a seperated function .This is because there are a lot of variables that needs to be updated in real time.
3) There is no way to know precisely the coordinate to put my text on the screen. So, I used trial and error to adjust the coordinate, which again, took a lot of time.

Week 7: April 11-April 18
- Integration Test --> Test all software function and calculation, update LCD display, test LED charge/discharge/warning status. And tried to upload all the data to show on NETPIE/mobilephone

-What I've done: Created interrupt switch function for emegency case. Created LED function status to display the operation mode of the BMS. and finally I tried to connected my BMS with NETPIE but still facing some error.

I've attached videos of integration test with all HW and SW, the green LED means the battery is being discharged (positive current) and the yellow LED means the battery is being charged (negative current). 

Whenever the user press button, the operation will stop immediately, no matter in what status the BMS is operating, and the LCD will show "Operation Teminated by User's Request, Contactor is Opened" 

Problem:
There is something wrong in the library of "ESP8266_Microgear". The error says "class BearSSL::WiFiClientSecure' has no member named 'verify'". I've searched for the solutions online, many people suggested me to downgrade the version of this library or replace the original "microgear.cpp" file with the one provided in Github.  I've tried every one of them but nothing works. 

Solution: I still couldn't figure out at this moment. But I already sent an email to Prof.Chayakorn for a help. 


 


