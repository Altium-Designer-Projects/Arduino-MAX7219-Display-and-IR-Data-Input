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
#define HARDWARE_TYPE MD_MAX72XX:: PAROLA_HW  // FC16_HW

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

//char displayBuffer[12];

char *displayBuffer[] ={"11:11:11","40:40:60"}; 

byte i;

void setup(void){
  P.begin();
}

void loop(void){
  
  if(P.displayAnimate()){
     
   P.displayText(displayBuffer[i], PA_CENTER, 20, 1000, PA_PRINT , PA_NO_EFFECT);

     delay(1000);
     i++;
     if( i >= 2){
         i=0;
      }
  
  }
  
}
