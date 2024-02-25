#include <ZsutDhcp.h>           
#include <ZsutEthernet.h>       
#include <ZsutEthernetUdp.h>    
#include <ZsutFeatures.h> 
#include <time.h>

#define UDP_SERVER_PORT         18566
#define PACKET_BUFFER_LENGTH        2

ZsutEthernetUDP Udp;
ZsutIPAddress addressIP(192, 168, 56, 102);
byte MAC[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01};
unsigned char packetBuffer[PACKET_BUFFER_LENGTH];
unsigned int localPort=UDP_SERVER_PORT;
uint16_t t;
unsigned int bufforLength=PACKET_BUFFER_LENGTH;
const unsigned long interval = 3500;
unsigned long previousMillis = 0;


void setup() {
  // Wiadomość Hello po uruchomieniu serwera
  Serial.begin(115200);
  Serial.print(F("Temperature Client Server init... ["));Serial.print(F(__FILE__));
  Serial.print(F(", "));Serial.print(F(__DATE__));Serial.print(F(", "));Serial.print(F(__TIME__));Serial.println(F("]")); 

  //inicjaja karty sieciowej    
  ZsutEthernet.begin(MAC);

  // Nasłuchiwanie na datagramy UDP
  Udp.begin(localPort);
}


void loop() {
  
  // Wyślij po UDP do klienta wiadomość o treści '2' - jestem dostępny
  packetBuffer[0] = '2';
  packetBuffer[1] = '0';
  Udp.beginPacket(addressIP, localPort);
  Udp.write(packetBuffer, bufforLength);
  Udp.endPacket();
  memset(packetBuffer, 0, sizeof(packetBuffer));


  if (Udp.available()) {  // Dodane sprawdzenie, czy dane są dostępne
    int packetSize = Udp.parsePacket();
    if (packetSize > 0) {
      int len = Udp.read(packetBuffer, PACKET_BUFFER_LENGTH);
      if (len <= 0) {
        Udp.flush();
        return;
      }

      Serial.print("Sent: ");
      packetBuffer[len] = '\0';
      Serial.println((char*)packetBuffer);

      // Wyślij po UDP do klienta wartość ostatniej zmierzonej temperatury
      if (packetBuffer[0] == '1') {
        int temp = randTemperature();
        temp = temp / 10;
        char tempString; 

        snprintf(tempString, sizeof(tempString), "%02d", temp);
        packetBuffer[0] = '3';
        packetBuffer[1] = tempString;
        
        Udp.beginPacket(addressIP, localPort);
        Udp.write(packetBuffer, bufforLength);
        Udp.endPacket();

        memset(packetBuffer, 0, sizeof(packetBuffer));
      } 
      else {
        // Nic nie rób
      }
    }
  }
}


int randTemperature() {
  srand(time(NULL));
  return rand() % 1001;
}
