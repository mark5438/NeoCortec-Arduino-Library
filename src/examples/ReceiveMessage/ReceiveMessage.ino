/*
 *  This example prints data received from NeoMesh network to Serial terminal
 */

#include <NeoMesh.h>

#define CTS_PIN 2

NeoMesh * neo;

void host_data(uint8_t n, tNcApiHostData * m)
{
    Serial.print("Data received from: ");
    Serial.println(m->originId);
    for(int i = 0; i < m->payloadLen; i++)
    {
        Serial.print(m->payload[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void setup()
{
    Serial1.begin(DEFAULT_NEOCORTEC_BAUDRATE);
    neo = new NeoMesh(&Serial1, CTS_PIN);
    neo->host_data_callback = host_data;
    neo->start();
}

void loop()
{
    neo->update();
}