#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "tuple.h"
#include "tuple_space.h"

#define PORT 5070
#define MAX_PACKET_SIZE 1024

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress server(192, 168, 50, 28); // Server IP address
unsigned int localPort = 5011;
EthernetUDP Udp;

void display_message_attributes(int id, field_t *fields, int num_fields, char *buffer, int buffer_len, int message_type)
{
    Serial.println("\n----Message-Attributes----");
    if (message_type == TS_OUT)
    {
        Serial.print("Message Type: ");
        Serial.println(message_type);
        Serial.print("ID: ");
        Serial.println(id);
        Serial.print("Number of fields: ");
        Serial.println(num_fields);
        for (int i = 0; i < num_fields; i++)
        {
            Serial.print("Field ");
            Serial.print(i);
            Serial.print(" type: ");
            Serial.println(fields[i].dataType);
            switch (fields[i].dataType)
            {
            case TYPE_INT:
                Serial.print("Field ");
                Serial.print(i);
                Serial.print(" value: ");
                Serial.println(fields[i].data.int_type);
                break;
            case TYPE_FLOAT:
                Serial.print("Field ");
                Serial.print(i);
                Serial.print(" value: ");
                Serial.println(fields[i].data.float_type);
                break;
            }
        }

        Serial.println("ALP Message Content:");
        for (int i = 0; i < buffer_len; i++)
        {
            Serial.print(String(buffer[i], HEX) + " ");
        }
        Serial.println();
    }
    else
    {
        Serial.print("Message Type: ");
        Serial.println(message_type);
        Serial.print("ID: ");
        Serial.println(id);
        Serial.print("Number of fields: ");
        Serial.println(num_fields);
        for (int i = 0; i < num_fields; i++)
        {
            Serial.print("Field ");
            Serial.print(i);
            Serial.print(" type: ");
            Serial.println(fields[i].dataType);
        }

        Serial.println("ALP Message Content:");
        for (int i = 0; i < buffer_len; i++)
        {
            Serial.print(String(buffer[i], HEX) + " ");
        }
        Serial.println();
    }
}

void initEthernet()
{
    Serial.begin(9600);
    Ethernet.begin(mac);
    Udp.begin(localPort);
}

void printHex(char *msg, int len)
{
    for (int i = 0; i < len; i++)
    {
        Serial.print(msg[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void send_message_to_server(char *msg, int len)
{
    if (Udp.beginPacket(server, PORT))
    {
        Udp.write(msg, len);
        Udp.endPacket();
    }
    else
    {
        Serial.println("Connection failed");
    }
}

int receive_message_from_server(char *msg)
{
    int len = Udp.parsePacket();
    if (len > 0)
    {
        Udp.read(msg, len);
        msg[len] = '\0';
        Serial.println("Received message from server:");
        Serial.println(msg);
    }

    return len;
}

int send_and_receive_message(char *msg, char *rcv_msg, int len)
{
    Udp.beginPacket(server, PORT);
    printHex(msg, len);
    Udp.write(msg, len);
    Udp.endPacket();

    delay(100); // Wait for a response

    int packetSize = receive_message_from_server(rcv_msg);

    if (packetSize > 1)
    {
        Serial.print("Received packet: ");
        Serial.println(packetSize);
        Serial.println(rcv_msg);
    }
    else
    {
        Serial.println("Waiting for a packet");
    }

    return packetSize;
}

int ts_out(int id, field_t *fields, int num_fields)
{
    initEthernet();
    char buffer[MAX_PACKET_SIZE];
    int len = 0;

    buffer[sizeof(long)] = TS_OUT;
    *((long *)(buffer)) = id;
    len += sizeof(long) + 1;

    buffer[len++] = (char)num_fields;
    for (int i = 0; i < num_fields; i++)
    {
        buffer[len++] = (char)fields[i].dataType;
    }

    for (int j = 0; j < num_fields; j++)
    {
        switch (fields[j].dataType)
        {
        case TYPE_INT:
            memcpy(buffer + len, (void *)&fields[j].data.int_type, sizeof(long));
            len += sizeof(long);
            break;
        case TYPE_FLOAT:
            memcpy(buffer + len, (void *)&fields[j].data.float_type, sizeof(float));
            len += sizeof(float);
            break;
        }
    }

    display_message_attributes(id, fields, num_fields, buffer, len, TS_OUT);

    send_message_to_server(buffer, len);

    return TS_SUCCESS;
}

int ts_inp(int id, field_t *fields, int num_fields)
{
    initEthernet();
    Serial.println("TS_INP_INIT");
    char buffer[MAX_PACKET_SIZE];
    char received_buffer[MAX_PACKET_SIZE];

    int len = 0;
    int received_message_len = 0;
    protocol_header *header;
    void *data_position;
    char *current_type;

    buffer[sizeof(long)] = TS_INP;
    *((long *)(buffer)) = id;
    len += sizeof(long) + 1;

    buffer[len++] = (char)num_fields;
    for (int i = 0; i < num_fields; i++)
    {
        buffer[len++] = (char)fields[i].dataType;
    }

    do
    {
        received_message_len = send_and_receive_message(buffer, received_buffer, len);
        if (received_message_len == 1)
        {
            delay(500);
            continue;
        }
        break;
    } while (1);

    for (int i = 0; i < received_message_len; i++)
    {
        Serial.print(String(received_buffer[i], HEX) + " ");
    }
    Serial.println();

    header = (protocol_header *)received_buffer;

    current_type = &header->data_type;
    data_position = current_type + num_fields;
    for (int j = 0; j < num_fields; j++)
    {
        switch (fields[j].dataType)
        {
        case TYPE_INT:
            memcpy(&fields[j].data.int_type, data_position, sizeof(long));
            data_position = reinterpret_cast<char *>(data_position) + sizeof(long);
            break;
        case TYPE_FLOAT:
            memcpy(&fields[j].data.float_type, data_position, sizeof(float));
            data_position = reinterpret_cast<char *>(data_position) + sizeof(float);
            break;
        }
    }

    return TS_SUCCESS;
}

int ts_rdp(int id, field_t *fields, int num_fields)
{
    initEthernet();
    char buffer[MAX_PACKET_SIZE];
    char received_buffer[MAX_PACKET_SIZE];

    int len = 0;
    int received_message_len = 0;
    protocol_header *header;
    void *data_position;
    char *current_type;

    buffer[sizeof(long)] = TS_RDP;
    *((long *)(buffer)) = id;
    len += sizeof(long) + 1;

    buffer[len++] = (char)num_fields;
    for (int i = 0; i < num_fields; i++)
    {
        buffer[len++] = (char)fields[i].dataType;
    }

    do
    {
        received_message_len = send_and_receive_message(buffer, received_buffer, len);
        if (received_message_len == 1)
        {
            delay(5000);
            continue;
        }
        break;
    } while (1);

    for (int i = 0; i < received_message_len; i++)
    {
        Serial.print(String(received_buffer[i], HEX) + " ");
    }
    Serial.println();

    header = (protocol_header *)received_buffer;

    current_type = &header->data_type;
    data_position = current_type + num_fields;
    for (int j = 0; j < num_fields; j++)
    {
        switch (fields[j].dataType)
        {
        case TYPE_INT:
            memcpy(&fields[j].data.int_type, data_position, sizeof(long));
            data_position = reinterpret_cast<char *>(data_position) + sizeof(long);
            break;
        case TYPE_FLOAT:
            memcpy(&fields[j].data.float_type, data_position, sizeof(float));
            data_position = reinterpret_cast<char *>(data_position) + sizeof(float);
            break;
        }
    }

    return TS_SUCCESS;
}
