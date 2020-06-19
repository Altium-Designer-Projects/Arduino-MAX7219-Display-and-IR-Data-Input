#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
/*
 Hello 
 I design developer tools for embedded electronic systems. You can buy my projects design files.
 https://www.altiumdesignerprojects.com
*/
#define CS_PIN        10
#define MAX_DEVICES   8
#define HARDWARE_TYPE MD_MAX72XX:: PAROLA_HW  

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char displayBuffer[12] = " ";

word counter = 0;
byte i = 0;

unsigned long interval = 500;
unsigned long previousMillis = 0;
unsigned long currentMillis;

void setup() {
 P.begin();
 P.setIntensity(6); 
 
}

void loop(){
  
 if (P.displayAnimate()){
  
   // sprintf(displayBuffer, "%s", "29.08.2019" ); 
    
   // sprintf(displayBuffer, "%d", counter );   
    
   sprintf(displayBuffer, "%04d", counter );  // 0001
   
   P.displayText(displayBuffer , PA_CENTER, 25, 100, PA_PRINT, PA_NO_EFFECT);

   //counter++;
   //delay(200);

   currentMillis = millis();
   if(currentMillis - previousMillis >= interval){
      counter++;
      previousMillis = currentMillis;                      
   }
   P.displayReset(); 
 }

}
