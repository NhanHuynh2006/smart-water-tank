// Buoc 7: kiem tra hai phao. Trang thai phai dao khi nhac phao len.
#include <Arduino.h>
#include "config.h"
void setup(){Serial.begin(115200);
  pinMode(PIN_FLOAT_MAX,INPUT_PULLUP);pinMode(PIN_FLOAT_SRC,INPUT_PULLUP);}
void loop(){
  Serial.printf("phao muc cao: %s | phao bon nguon: %s\n",
    digitalRead(PIN_FLOAT_MAX)==LOW?"KICH HOAT":"binh thuong",
    digitalRead(PIN_FLOAT_SRC)==LOW?"con nuoc":"CAN NUOC");
  delay(500);
}
