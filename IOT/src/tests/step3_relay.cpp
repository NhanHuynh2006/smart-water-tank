// Buoc 3: dong cat relay 2 giay mot lan, CHUA NOI BOM.
#include <Arduino.h>
#include "config.h"
void setup(){Serial.begin(115200);pinMode(PIN_RELAY,OUTPUT);}
void loop(){
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY,LOW);
#else
  digitalWrite(PIN_RELAY,HIGH);
#endif
  Serial.println("relay ON");delay(2000);
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY,HIGH);
#else
  digitalWrite(PIN_RELAY,LOW);
#endif
  Serial.println("relay OFF");delay(2000);
}
