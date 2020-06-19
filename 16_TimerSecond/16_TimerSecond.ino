/*
 Hello 
 I design developer tools for embedded electronic systems. You can buy my projects design files.
 https://www.altiumdesignerprojects.com
*/

word milliSecond = 0;  
bool timerStart = false;

byte Second = 0;
byte lastSecond;

char rx_byte;

void setup(){

 Serial.begin(9600);
 
 noInterrupts(); // disable all interrupts // MEGA2560 
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
 
 if(Serial.available() > 0){       
    rx_byte = Serial.read();       
    if(rx_byte == '1'){
       timerStart = true;  
    }
    
    if(rx_byte == '2'){
       timerStart = false;   
    }
    
    if(rx_byte == '3'){
       milliSecond = 0;
       Second = 0;  
    }
 }
 
 if(timerStart == true){ 
  if(milliSecond >= 1000){ 
     milliSecond = 0;
     Second++;
  }
 }
 
 if(Second != lastSecond){ 
    Serial.print("Second .... ");  Serial.println(Second);
 }
 lastSecond = Second; 

}

ISR(TIMER3_COMPA_vect){   

 if(timerStart == true){ 
    milliSecond++;
 }
 
}  // ISR(TIMER3_COMPA_vect)
