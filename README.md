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

