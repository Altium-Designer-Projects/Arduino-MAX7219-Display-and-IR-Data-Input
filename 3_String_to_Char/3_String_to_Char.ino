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

String cityString = "istanbul";  // string data

char cityChar[] = "istanbul";  // array char data

void setup(){
 P.begin();
}

void loop(){
  
 if(P.displayAnimate()){
   cityString.toCharArray(displayBuffer , cityString.length() + 1 );
   P.displayText(displayBuffer, PA_CENTER, 25, 100, PA_PRINT, PA_NO_EFFECT);
 }
  
}
