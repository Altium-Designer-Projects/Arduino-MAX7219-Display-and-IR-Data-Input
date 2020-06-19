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

char displayBuffer[12];

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 };  // Deg C
uint8_t degF[] = { 6, 3, 3, 124, 20, 20, 4 };  // Deg F

byte i = 0;

void setup(){
  
  P.begin();
  P.addChar('$', degC); 
  P.addChar('&', degF);
 
}

void loop(){
  
  if(P.displayAnimate()){
    
   //strcpy(displaybuffer, "22.50 $");   //  cantigrad
   
   strcpy(displayBuffer, "71.6 &");      //  fahre
   
   P.displayText(displayBuffer , PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

  }
  
}
