
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <EEPROM.h>
#include <DS3231.h>
/*
 Hello 
 I design developer tools for embedded electronic systems. You can buy my projects design files.
 https://www.altiumdesignerprojects.com
*/
DS3231 rtc(SDA, SCL);
Time  t;

#include <IRremote.h>
const int RECV_PIN = 8;

IRrecv irrecv(RECV_PIN);
decode_results results;

#define CS_PIN        10
#define MAX_DEVICES   8
#define HARDWARE_TYPE MD_MAX72XX:: PAROLA_HW  

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char displayBuffer[20];

char *daysName[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"}; 

byte dataDirection;

byte timeHour;
byte timeMinute;
byte timeSecond;
byte timeDay;
byte timeMonth;  
byte timeDate;
word timeYear;

byte testValue = 0;

unsigned long resultCode;
unsigned long lastCode = 0; 

word milliSecond = 0;  
bool timerStart = false;

bool irSignalInput = false;

bool timeSync = true;
bool eepromControl = true;

const byte buttonHold = 20;

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
 //Serial.begin(9600);
 //Serial.println("start");
 irrecv.enableIRIn();
 irrecv.blink13(true);
 
 rtc.begin();

 P.begin();
 P.setIntensity(6); 

 for(byte i; i < 20; i++){
   displayBuffer[i] = '\0';
 }
 timeHour   = 12;
 timeMinute = 12;
 timeSecond = 0;
 
 timeDay    = 1; // salı
 
 timeMonth  = 1;  
 timeDate   = 1; 
 timeYear   = 2010;

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

void loop(){

 if(eepromControl == true){  
   if( EEPROM.read(0) != 160 && EEPROM.read(1) != 180 ){
      rtc.setDOW(MONDAY);       
      rtc.setTime(12,12,0);    
      rtc.setDate(1,1,2018);  
   }
   eepromControl = false;
 }
   
 if(irrecv.decode(&results)){
    irSignalInput = true;
    resultCode = results.value & REPEAT;
   
   if(resultCode == REPEAT){
      resultCode = lastCode;
   }else{
      lastCode = resultCode;
   }
  
   irrecv.resume();
 }else{
   
 } // if(irrecv.decode(&results)) bitiyor

 // *********************
 
 if(P.displayAnimate()){
    P.displayText(displayBuffer , PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    
  t = rtc.getTime();
    
  if(timeSync == true){ 
     timeHour   = t.hour; 
     timeMinute = t.min;
     timeSecond = t.sec;
     timeDay    = t.dow;
     timeMonth  = t.mon;
     timeDate   = t.date;
     timeYear   = t.year;
     timeSync   = false;   //  enter dan sonra true yap
  }

  switch(dataDirection){
   case 0:
    displayBuffer[0] = timeHour / 10 + 48;
    displayBuffer[1] = timeHour % 10 + 48;
    displayBuffer[2] = ':';
    displayBuffer[3] = '-';
    displayBuffer[4] = '-';
    displayBuffer[5] = ':';
    displayBuffer[6] = '-';
    displayBuffer[7] = '-';
    displayBuffer[8] = '\0';  
    displayBuffer[9] = '\0';     
    break;
    case 1:
    displayBuffer[0] = '-';
    displayBuffer[1] = '-';
    displayBuffer[2] = ':';
    displayBuffer[3] = timeMinute / 10 + 48;
    displayBuffer[4] = timeMinute % 10 + 48;
    displayBuffer[5] = ':';
    displayBuffer[6] = '-';
    displayBuffer[7] = '-';
    displayBuffer[8] = '\0';
    displayBuffer[9] = '\0';     
    break;
   case 2:
    displayBuffer[0] = '-';
    displayBuffer[1] = '-';
    displayBuffer[2] = ':';
    displayBuffer[3] = '-';
    displayBuffer[4] = '-';
    displayBuffer[5] = ':';
    displayBuffer[6] = timeSecond / 10 + 48;
    displayBuffer[7] = timeSecond % 10 + 48;
    displayBuffer[8] = '\0';
    displayBuffer[9] = '\0';     
    break;
   case 3:        
    sprintf(displayBuffer, "%s", daysName[timeDay - 1]);  // - 1 önemli
    break;
   case 4:
    displayBuffer[0] = timeMonth / 10 + 48;
    displayBuffer[1] = timeMonth % 10 + 48;
    displayBuffer[2] = '.';
    displayBuffer[3] = '-';
    displayBuffer[4] = '-';
    displayBuffer[5] = '.';
    displayBuffer[6] = '-';
    displayBuffer[7] = '-';
    displayBuffer[8] = '-';
    displayBuffer[9] = '-'; 
    break;
   case 5:
    displayBuffer[0] = '-';
    displayBuffer[1] = '-';
    displayBuffer[2] = '.';
    displayBuffer[3] = timeDate / 10 + 48;
    displayBuffer[4] = timeDate % 10 + 48;
    displayBuffer[5] = '.';
    displayBuffer[6] = '-';
    displayBuffer[7] = '-';
    displayBuffer[8] = '-';
    displayBuffer[9] = '-'; 

    break;
   case 6:
    displayBuffer[0] = '-';
    displayBuffer[1] = '-';
    displayBuffer[2] = '.';
    displayBuffer[3] = '-';
    displayBuffer[4] = '-';
    displayBuffer[5] = '.';
    displayBuffer[6] = timeYear / 1000 % 10 + 48;
    displayBuffer[7] = timeYear / 100  % 10 + 48;
    displayBuffer[8] = timeYear / 10   % 10 + 48;
    displayBuffer[9] = timeYear % 10 + 48;  
    break;    
  }
  
  switch(dataDirection){
   case 0:  // --------------  saat 
     if(irSignalInput == true && resultCode == buttonUp){ 
        timerStart = true;      
      if(milliSecond >= buttonHold){
         milliSecond = 0;
         timerStart = false;
         timeHour++; 
       }
       
       if(timeHour >= 24){
          timeHour = 0;
       } 
       irSignalInput = false;
     }
     
     if(irSignalInput == true && resultCode == buttonDown){ 
        timerStart = true;       
      if(milliSecond >= buttonHold){
         milliSecond = 0;
         timerStart = false;
         timeHour--; 
       }
       if(timeHour >= 255){
          timeHour = 23;
       }
       irSignalInput = false;             
     }
    break;
    case 1: // dakika
     if(irSignalInput == true && resultCode == buttonUp){
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeMinute++; 
        }
        if(timeMinute >= 60){
           timeHour++;
           timeMinute = 0;
        }
       irSignalInput = false; 
     }
        
     if(irSignalInput == true && resultCode == buttonDown){ 
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeMinute--; 
        }
        if(timeMinute >= 255){
           timeMinute = 59;        
           timeHour--;
        }
        irSignalInput = false;             
     }
    break;
   case 2:   // saniye
     if(irSignalInput == true && resultCode == buttonUp){
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeSecond++; 
        }
     
        if(timeSecond >= 60){
           timeSecond = 0;        
           timeMinute++;
        }  
        irSignalInput = false;             
     }
         
     if(irSignalInput == true && resultCode == buttonDown){ 
           timerStart = true;  
           if(milliSecond >= buttonHold){
              milliSecond = 0;
              timerStart = false;
              timeSecond--; 
           }
     
           if(timeSecond >= 255){
              timeSecond = 59;        
              timeMinute--;
           } 
           irSignalInput = false;             
     }
    break;
   case 3:     // day    
     if(irSignalInput == true && resultCode == buttonUp){
        timerStart = true;  
        if(milliSecond >= 30){
           milliSecond = 0;
           timerStart = false;
           timeDay++; 
        }
          
        if(timeDay >= 8){
           timeDay = 1;
        } 
        irSignalInput = false;              
     }
         
     if(irSignalInput == true && resultCode == buttonDown){ 
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeDay--; 
        }         
        if(timeDay < 1){
           timeDay = 7;
        } 
        irSignalInput = false;             
     }
    break;
   case 4:  // month
     if(irSignalInput == true && resultCode == buttonUp){
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeMonth++;  
        } 
         
        if(timeMonth >= 13){
           timeMonth = 1;
        } 
        irSignalInput = false;                
     }
         
     if(irSignalInput == true && resultCode == buttonDown){ 
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeMonth--;  
        }
       
        if(timeMonth <= 0 ){  // 252 253 254 255 0 1 2 3  
           timeMonth = 12;
        }  
        irSignalInput = false;     
     }
    break;
   case 5: // year
     if(irSignalInput == true && resultCode == buttonUp){
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeDate++;  
        } 
        if(timeDate >= 32){
           timeDate = 1;
        }
        irSignalInput = false;              
     }
         
     if(irSignalInput == true && resultCode == buttonDown){ 
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeDate--;  
        }
                    
        if(timeDate <= 0){
           timeDate = 31;
        } 
        irSignalInput = false;  
     }
    break;
   case 6:  // year
     if(irSignalInput == true && resultCode == buttonUp){
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeYear++; 
        }
           
        if(timeYear >= 2099){
           timeYear = 2010;
        } 
        irSignalInput = false;             
     }
         
     if(irSignalInput == true && resultCode == buttonDown){ 
        timerStart = true;  
        if(milliSecond >= buttonHold){
           milliSecond = 0;
           timerStart = false;
           timeYear--; 
        }          
        if(timeYear <= 1950){
           timeYear = 2099;
        } 
        irSignalInput = false;                        
     }

    break;    
  }

   if(irSignalInput == true && resultCode == buttonRight){  
      timerStart = true;
      if(milliSecond >= buttonHold){
         milliSecond = 0;
         timerStart = false;
         dataDirection++;
       }
    
       if(dataDirection >= 7){
          dataDirection = 0;   
       }
       irSignalInput = false;
   }
   
   if(irSignalInput == true && resultCode == buttonLeft){ 
      timerStart = true;
      if(milliSecond >= buttonHold){
         milliSecond = 0;
         timerStart = false;
         dataDirection--;
       }
    
       if(dataDirection >= 255){
          dataDirection = 6;   
       }
       irSignalInput = false;     
   }
   
  if(irSignalInput == true && resultCode == buttonEnter){
      timerStart = true;

      if(milliSecond >= 1000){
         milliSecond = 0;
         timerStart = false;
       
         rtc.setTime(timeHour, timeMinute, timeSecond);    
         rtc.setDOW(timeDay); 
         rtc.setDate(timeMonth, timeDate, timeYear); 
       
         //delay(250);
         EEPROM.write(0 , 160);
         EEPROM.write(1 , 180);
         eepromControl = true;
         timeSync = true;
     
         //systemMainLoop = true;      // projenin son aşamasında eklenecek
         //systemSettingsLoop = false;
      }
      
      irSignalInput = false;
   }

 } // if(P.displayAnimate())   bitiyor

  
} // loop bitiyor

// ***********************  Functions **************************


ISR(TIMER3_COMPA_vect){   

 if(timerStart == true){ 
    milliSecond++;
   
 }
 
}
