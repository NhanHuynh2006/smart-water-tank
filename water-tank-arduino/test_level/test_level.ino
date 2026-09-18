// Buoc 2: chi kiem tra cam bien sieu am. Doc phai khop thuoc o ba diem.
#include <Arduino.h>
#include "config.h"
void setup(){Serial.begin(115200);pinMode(PIN_TRIG,OUTPUT);pinMode(PIN_ECHO,INPUT);}
void loop(){
  digitalWrite(PIN_TRIG,LOW);delayMicroseconds(4);
  digitalWrite(PIN_TRIG,HIGH);delayMicroseconds(10);digitalWrite(PIN_TRIG,LOW);
  uint32_t d=pulseIn(PIN_ECHO,HIGH,30000UL);
  if(!d){Serial.println("khong co echo, kiem tra day va chia ap");}
  else{float cm=d*0.0343f/2.0f;
    Serial.printf("khoang cach %.1f cm  |  muc nuoc %.1f cm\n",cm,TANK_SENSOR_TO_BOTTOM_CM-cm);}
  delay(500);
}
