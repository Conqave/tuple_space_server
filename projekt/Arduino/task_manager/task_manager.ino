#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "tuple.h"
#include "tuple_space.h"

//#define SERVER "192.168.0.177"  // Adres IP serwera
//#define PORT 5011

#define MAX_FIELDS 10

#define REMAINDER 0
#define ID 1

void setup() {
  randomSeed(analogRead(0));
}

void loop() {

    field_t fields[2];
    field_t templateArray[1];

        templateArray[0].dataType = TYPE_INT;

        Serial.print("\nType: ");
        Serial.println(templateArray[0].dataType);
        
        Serial.print("\nDownloaded Data: ");
        Serial.println(templateArray[0].data.int_type);


        fields[0].dataType = TYPE_FLOAT;
        fields[0].data.float_type = 18.75;
        fields[1].dataType = TYPE_FLOAT;
        fields[1].data.float_type = 0.32;
        
        delay(3000);

        ts_out(ID, fields, 2);
    //}
}
