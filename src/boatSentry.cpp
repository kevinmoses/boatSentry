#include "application.h"
#line 1 "/home/kmoses/boatSentry/src/boatSentry.ino"
void setup();
void loop();
void checkTriggers();
unsigned long checkTime(String message, unsigned long delay, unsigned long lastPublish);
void readSensors();
void prepAndSendData(String message);
#line 1 "/home/kmoses/boatSentry/src/boatSentry.ino"
float waterRaw = 0;
float batt1Raw = 0;
float batt2Raw = 0;
float bilgePumpRaw = 0;
float voltMeterRaw = 0;
float water = 0;
float batt1 = 0;
float batt2 = 0;
float bilgePump = 0;
float voltMeter = 0;
float minVoltage= 12.3;
float bilgePumpTrigger= 10;
int alarmMode = 0; //0= no alarm, 1= voltage, 2 =bilge pump on, 3= bilge overtime or water in bilge
unsigned long publishDelayBatt = 120000;
unsigned long publishDelayPump = 20000;
unsigned long publishDelayWater = 200000;
unsigned long publishDelayConst = 60000*60;
//unsigned long publishDelayConst = 5000;
unsigned long lastPublishConst = 0;
unsigned long lastPublishBatt = 0;
unsigned long lastPublishPump = 0;
unsigned long lastPublishWater = 0;
int bilgePumpRunTimeTrigger = 60000; //=1min
int bilgeRunTime = 0; //last 6 hours



void setup() {
analogWrite(DAC, 4095);
readSensors();
prepAndSendData("BoatData");
}

void loop() {
readSensors();
checkTriggers();
delay(500); 
}

void checkTriggers(){

if (water>=1000 || bilgeRunTime >= bilgePumpRunTimeTrigger){
if (alarmMode != 3){
alarmMode = 3;
lastPublishWater =  checkTime("BoatDataWaterInBoat", 0, lastPublishWater);
delay(2000);
}else
{
lastPublishWater = checkTime("BoatDataWaterInBoat",publishDelayWater, lastPublishWater);
}

}else if (bilgePump>= bilgePumpTrigger){
  if(alarmMode !=2)
  {
alarmMode = 2;
lastPublishPump = checkTime("BoatDataBilgeOn", 0, lastPublishPump);
delay(1000);
  }else
  {
 lastPublishPump = checkTime("BoatDataBilgeOn", publishDelayPump, lastPublishPump);
  }
  

}else if (batt1<=minVoltage || batt2 <=minVoltage){
 if(alarmMode != 1){
  lastPublishBatt = checkTime("BoatDataBatteryALERT!",0, lastPublishBatt);
  alarmMode = 1;
  delay(30000);
 }else
 {
     lastPublishBatt = checkTime("BoatDataBatteryALERT!",publishDelayBatt, lastPublishBatt);
 }
  
}else {
  alarmMode = 0;
}lastPublishConst = checkTime("BoatData", publishDelayConst, lastPublishConst);

}

unsigned long checkTime(String message, unsigned long delay, unsigned long lastPublish){
unsigned long now = millis();

        if ((now - lastPublish) > delay) {
           prepAndSendData(message);
           lastPublish = now;
        }return lastPublish;
    }



void readSensors(){
waterRaw = analogRead(A1);
batt1Raw = analogRead(A2);
batt2Raw = analogRead(A3);
bilgePumpRaw = analogRead(A4);
voltMeterRaw = analogRead(A5);
water = waterRaw;
batt1 = batt1Raw/261;
batt2 = batt2Raw/261;
bilgePump = bilgePumpRaw/261;
voltMeter = voltMeterRaw/261;
}

void prepAndSendData(String message){
  char resultstr[128];
    strcpy (resultstr,"{\"Battery 1\":");
    strcat (resultstr,String(batt1, 2));
    strcat (resultstr,", \"Battery 2\":");
    strcat (resultstr,String(batt2, 2));
    strcat (resultstr,", \"Bilge Status\":");
    strcat (resultstr,String(bilgePump, 0));
    strcat (resultstr,", \"Water Sensor\":");
    strcat (resultstr,String(water, 2));
    strcat (resultstr,", \"voltMeter\":");
    strcat (resultstr,String(voltMeter, 2));
    strcat (resultstr,"}");
    Particle.publish(String(message), resultstr, PRIVATE);
}