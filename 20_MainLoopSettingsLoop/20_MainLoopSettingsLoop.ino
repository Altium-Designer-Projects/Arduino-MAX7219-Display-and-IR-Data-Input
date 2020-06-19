#include <IRremote.h>
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

const int RECV_PIN = 8;

IRrecv irrecv(RECV_PIN);
decode_results results;

#define CS_PIN        10
#define MAX_DEVICES   8
#define HARDWARE_TYPE MD_MAX72XX:: PAROLA_HW  

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char displayBuffer[20];

unsigned long resultCode;
unsigned long lastCode = 0; 

bool irSignalInput = false;
unsigned long milliSecond = 0;  
bool timerStart = false;

bool systemMainLoop     = true;  // normal çalışma
bool systemSettingsLoop = false; // ayarlar kısmı

bool mainLoop = true;  

#define buttonUp          0XFF629D
#define buttonDown        0XFFA857
#define buttonRight       0XFFC23D
#define buttonLeft        0XFF22DD
#define buttonEnter       0XFF02FD

#define button1           0XFF6897
#define button2           0XFF9867
#define button3           0XFFB04F
#define button4           0XFF30CF
#define button5           0XFF18E7
#define button6           0XFF7A85
#define button7           0XFF10EF
#define button8           0XFF38C7
#define button9           0XFF5AA5
#define button10          0XFF4AB5
#define asteriskButton    0XFF42BD
#define numberSignButton  0XFF52AD

void setup(){
 Serial.begin(9600);
 Serial.println("start");
 irrecv.enableIRIn();
 irrecv.blink13(true);
 
 rtc.begin();
 rtc.setDOW(MONDAY);     
 rtc.setTime(12, 30, 0);    
 rtc.setDate(5, 15, 2020);  
  
 P.begin();
 P.setIntensity(6); 
 
 for(byte i; i < 20; i++){
   displayBuffer[i] = '\0';
 }

 noInterrupts(); // disable all interrupts
 TCCR3A = 0;
 TCCR3B = 0;
 TCNT3  = 0;
 OCR3A  = 249;                         // 1000 Hz (16000000/((249+1)*64))    1kHz  // 1 ms
 TCCR3B |= (1 << WGM32);               // CTC
 TCCR3B |= (1 << CS31) | (1 << CS30);  // Prescaler 64
 TIMSK3 |= (1 << OCIE3A);              // Output Compare Match A Interrupt Enable
 interrupts();  // enable
 
}

void loop() {

 if(irrecv.decode(&results)){
    irSignalInput = true;
    resultCode = results.value & REPEAT;
   
    if(resultCode == REPEAT){
       resultCode = lastCode;
    }else{
       lastCode = resultCode;
    }
     
    irrecv.resume();
 } //  if(irrecv.decode(&results)) bitiyor


 if(P.displayAnimate()){
    P.displayText(displayBuffer, PA_CENTER, 25, 100, PA_PRINT, PA_NO_EFFECT);

  if(systemMainLoop == true && systemSettingsLoop == false){   // main loop
    if(mainLoop == true){
       Serial.println("\r\n--- MAIN LOOP ---\r\n");
       milliSecond = 0;
       mainLoop = false;
    }

    strcpy(displayBuffer, "main loop"); 
    

    if(irSignalInput == true && resultCode == button1){
       timerStart = true;

     if(milliSecond >= 1000){ 
        milliSecond = 0;
        timerStart = false;
        systemMainLoop = false; 
        systemSettingsLoop = true;
     }
     irSignalInput = false;
    }
   
  }  // main loop bitiyor *******

  // **************************

  if(systemMainLoop == false && systemSettingsLoop == true){   // settings loop
    if(mainLoop == false){
       Serial.println("\r\n--- SETTING LOOP ---\r\n"); 
       milliSecond = 0;
       mainLoop = true;
    }
   
    strcpy(displayBuffer, "settings"); 

    if(irSignalInput == true && resultCode == button3){ // enter 
       timerStart = true;

    if(milliSecond >= 1000){
       milliSecond = 0;
       timerStart = false;
       systemMainLoop = true; 
       systemSettingsLoop = false;
    }
      irSignalInput = false;
   }

  } // settings loop bitiyor ********
   
 } // if(P.displayAnimate())  bitiyor

} /// loop bitiyor


ISR(TIMER3_COMPA_vect){   

 if(timerStart == true){ 
    milliSecond++;
 }
 
}  // ISR(TIMER1_COMPA_vect)  ********
