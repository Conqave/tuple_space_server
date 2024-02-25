#include <ZsutEthernet.h>
#include <ZsutEthernetUdp.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
ZsutIPAddress ip(192, 168, 56, 102);

unsigned int localPort = 15225;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
ZsutEthernetUDP Udp;
int buzzer = 2;
void setup() {

  pinMode(buzzer, OUTPUT);
  // start the Ethernet
  ZsutEthernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start UDP
  Udp.begin(localPort);
  Serial.println("Hej!");
}

void loop() {

  // Wysłanie wiadomości o byciu aktywnym do klienta
  memset(packetBuffer, 0, sizeof(packetBuffer));  // Czyszczenie bufora
  packetBuffer[0] = '2';  // Dodanie '2' do bufora
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(packetBuffer, UDP_TX_PACKET_MAX_SIZE); // Wysłanie bufora
  Udp.endPacket();

  // Odczyt wiadomości przysłanej przez 
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    ZsutIPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    char packetValue = packetBuffer[0];
    Serial.println(packetValue);
    if(packetValue=='1'){
      Serial.println("Odgrywam dźwięk");
      tone(buzzer,440, 3500);
      }else {
      Serial.println("Unknown command");
    }
    if(packetValue=='0'){
      Serial.println("Czy jestem dostępny");
      delay(100);
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write('20');
      Udp.endPacket();
      }

  }
  delay(10);
}
