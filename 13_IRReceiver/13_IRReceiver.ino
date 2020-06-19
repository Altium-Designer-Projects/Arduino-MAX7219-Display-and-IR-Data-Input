#include <IRremote.h>
/*
 Hello 
 I design developer tools for embedded electronic systems. You can buy my projects design files.
 https://www.altiumdesignerprojects.com
*/
const int RECV_PIN = 8;

IRrecv irrecv(RECV_PIN);
decode_results results;

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
}

void loop(){
  
  if(irrecv.decode(&results)){
     Serial.println(results.value, HEX);
      
      /*
      if(results.value == REPEAT){
         Serial.println("REPEAT");
      }
      */
      
     irrecv.resume();
  }

}
