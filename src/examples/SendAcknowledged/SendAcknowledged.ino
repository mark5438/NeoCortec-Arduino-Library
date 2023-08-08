/*
 *  This example will send a test array as an acknowledged message to a 
 *  NeoCortec node every 5 seconds and print a message to the serial port
 * when the recepient node has acknowledged the message
 */
#include <NeoMesh.h>

#define RECEPIENT_NODE_ID 0x0010
#define CTS_PIN 2

NeoMesh * neo;
uint32_t last;

uint8_t test[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

void acknowledged(uint8_t n, tNcApiHostAckNack * m)
{
    Serial.print("Node ");
    Serial.print(m->originId);
    Serial.println(" acknowledged a message");
}

void setup()
{
    Serial1.begin(DEFAULT_NEOCORTEC_BAUDRATE);
    neo = new NeoMesh(&Serial1, CTS_PIN);
    neo->host_ack_callback = acknowledged;
    neo->start();
}

void loop()
{
    neo->update();

    if (millis() - last > 5000)
    {
        neo->send_acknowledged(RECEPIENT_NODE_ID, 0, test, 10);
        last = millis();
    }

}