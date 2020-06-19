
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
//    daysName[0] = Monday

byte dataDirection;

byte timeHour;
byte timeMinute;
byte timeSecond;
byte timeDay;
byte timeMonth;  
byte timeDate;
word timeYear;

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Deg C
uint8_t degF[] = { 6, 3, 3, 124, 20, 20, 4 }; // Deg F

float tempRead;
byte dspScene  = 0;
byte dspEffect = 0;

const byte buttonHold = 20;

unsigned long resultCode;
unsigned long lastCode = 0; 

bool irSignalInput = false;
word milliSecond = 0;  
bool timerStart = false;

bool systemMainLoop     = true;  // normal çalışma
bool systemSettingsLoop = false; // ayarlar kısmı

bool mainLoop = true;  

bool timeSync = true;
bool eepromControl = true;

#define buttonUp          0XFF629D
#define buttonDown        0XFFA857
#define buttonRight       0XFFC23D
#define buttonLeft        0XFF22DD
#define buttonEnter       0XFF02FD

#define button1           0XFF6897  // settings button
#define button2           0XFF9867  
#define button3           0XFFB04F  // enter button
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
 //irrecv.blink13(true);
 
 rtc.begin();
  
 P.begin();
 //P.setIntensity(6); 
 P.addChar('$', degC); 
 P.addChar('&', degF);
 
 for(byte i = 0; i < 20; i++){
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

void loop(){

 if(eepromControl == true){  
   if( EEPROM.read(0) != 140 && EEPROM.read(1) != 180 ){
      rtc.setDOW(MONDAY);       
      rtc.setTime(12,12,12);    
      rtc.setDate(1,1,2018);  
   }
   eepromControl = false;  // enter butonundan sonra true yap
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
 } //  if(irrecv.decode(&results))


 if(P.displayAnimate()){
    P.displayText(displayBuffer, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

 if(systemMainLoop == true && systemSettingsLoop == false){   // main loop
  if(mainLoop == true){
     Serial.println("\r\n--- MAIN LOOP ---\r\n");
     P.setSpeed(40);          
     P.setPause(500);     
     P.setTextEffect(0, PA_SCROLL_DOWN, PA_NO_EFFECT);     
     milliSecond = 0;
     mainLoop = false;
  }

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
    

   if(irSignalInput == true && resultCode == button1){ // ayarlar bölümüne gidiyoruz
      timerStart = true;

    if(milliSecond >= 750){ 
       milliSecond = 0;
       timerStart = false;
       systemMainLoop = false; 
       systemSettingsLoop = true;
    }
     irSignalInput = false;
   }
   
 }  // main loop bitiyor *******

// **********************************************************
// **********************************************************

  if(systemMainLoop == false && systemSettingsLoop == true){   // settings loop
   if(mainLoop == false){
      Serial.println("\r\n--- SETTING LOOP ---\r\n");
      P.setSpeed(40);          
      P.setPause(500);     
      P.setTextEffect(0, PA_SCROLL_UP, PA_NO_EFFECT);       
      milliSecond = 0;
      mainLoop = true;
   }
   
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
    displayBuffer[0]  = timeHour / 10 + 48;
    displayBuffer[1]  = timeHour % 10 + 48;
    displayBuffer[2]  = ':';
    displayBuffer[3]  = '-';
    displayBuffer[4]  = '-';
    displayBuffer[5]  = ':';
    displayBuffer[6]  = '-';
    displayBuffer[7]  = '-';
    displayBuffer[8]  = '\0'; 
    displayBuffer[9]  = '\0';
    displayBuffer[10] = '\0'; 
    displayBuffer[11] = '\0';  
          
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
    displayBuffer[8]  = '\0'; 
    displayBuffer[9]  = '\0';
    displayBuffer[10] = '\0'; 
    displayBuffer[11] = '\0';      
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
    displayBuffer[8]  = '\0'; 
    displayBuffer[9]  = '\0';
    displayBuffer[10] = '\0'; 
    displayBuffer[11] = '\0';      
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
    displayBuffer[8]  = '\0'; 
    displayBuffer[9]  = '\0';
    displayBuffer[10] = '\0'; 
    displayBuffer[11] = '\0'; 
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
    displayBuffer[8]  = '\0'; 
    displayBuffer[9]  = '\0';
    displayBuffer[10] = '\0'; 
    displayBuffer[11] = '\0';  

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
    displayBuffer[10] = '\0'; 
    displayBuffer[11] = '\0';      
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
   
   
  if(irSignalInput == true && resultCode == button3){  // enter butonu
      timerStart = true;

      if(milliSecond >= 750){
         milliSecond = 0;
         timerStart = false;
       
         rtc.setTime(timeHour, timeMinute, timeSecond);    
         rtc.setDOW(timeDay); 
         rtc.setDate(timeMonth, timeDate, timeYear); 
            
         EEPROM.write(0 , 140);
         EEPROM.write(1 , 180);
         dataDirection = 0;
         eepromControl = true;
         timeSync = true;
         
         mainLoop = true;
         systemMainLoop = true;      // projenin son aşamasında eklenecek
         systemSettingsLoop = false;
      }
      
      irSignalInput = false;
   }

  } // settings loop bitiyor ********


   P.displayReset();
 } // if(P.displayAnimate())  bitiyor

} /// loop bitiyor


ISR(TIMER3_COMPA_vect){   

 if(timerStart == true){ 
    milliSecond++;
   
 }
 
}  // ISR(TIMER1_COMPA_vect)  ********

  
