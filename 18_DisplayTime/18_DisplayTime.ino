#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <DS3231.h>
/*
 Hello 
 I design developer tools for embedded electronic systems. You can buy my projects design files.
 https://www.altiumdesignerprojects.com
*/

DS3231 rtc(SDA, SCL);
Time  t;

#define CS_PIN        10
#define MAX_DEVICES   8
#define HARDWARE_TYPE MD_MAX72XX:: PAROLA_HW  

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char displayBuffer[20]; 

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Deg C

float tempRead;
byte dspScene  = 0;
byte dspEffect = 0;

void setup(void){
 P.begin();
 rtc.begin();
 rtc.setDOW(MONDAY);     
 rtc.setTime(14, 59, 0);    
 rtc.setDate(26, 1, 2020);  
  
 P.begin();
 P.setIntensity(6); 
 P.addChar('$', degC); 
}

void loop(void){
  
 if(P.displayAnimate()){
    P.displayText(displayBuffer, PA_CENTER, 25, 100, PA_PRINT, PA_NO_EFFECT);

    t = rtc.getTime();
   
    switch(dspScene){
      case 0:
      strcpy(displayBuffer,rtc.getTimeStr());
      if(dspEffect == 0){
         P.setTextEffect(PA_OPENING, PA_NO_EFFECT);  
         dspEffect = 1;
       }
       break;
      case 1:
       strcpy(displayBuffer, rtc.getDOWStr()); 
       if(dspEffect == 1){
          P.setTextEffect(PA_RANDOM, PA_NO_EFFECT);
          dspEffect = 2;
       } 
      break;
      case 2:
       strcpy(displayBuffer,rtc.getDateStr());
       if(dspEffect == 2){
          P.setTextEffect(PA_SCROLL_UP, PA_NO_EFFECT);
          dspEffect = 3;
       }  
      break;
      case 3:
       tempRead = rtc.getTemp();
       dtostrf(tempRead, 4, 2, displayBuffer);
       strcat(displayBuffer, " $");
   
       if(dspEffect == 3){
          P.setTextEffect(PA_SCROLL_DOWN, PA_NO_EFFECT);
          dspEffect = 0;
       }
      break;
    }
    
   // t.sec  0    30     31   40     41   50     51    59 
   //         time         dow         date        temp

   if(t.sec >= 0 && t.sec <= 30){
      dspScene = 0;
   }else if(t.sec >= 31 && t.sec <= 40){
      dspScene = 1;
   }else if(t.sec >= 41 && t.sec <= 50){
      dspScene = 2;
   }else if(t.sec >= 51 && t.sec <= 60){
      dspScene = 3;  
   }
     
 }

}
