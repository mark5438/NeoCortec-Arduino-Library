# NeoCortec Arduino Library

Communicate over a NeoMesh network using an Arduino and this super user friendly library.

## Examples


### Send acknowledged messages
```
/*
 *  This example will send a test array as an acknowledged message to a 
 *  NeoCortec node every 5 seconds and print a message to the serial port
 * when the recepient node has acknowledged the message
 */
#include <NeoMesh.h>

#define RECEPIENT_NODE_ID 16
#define CTS_PIN 2

NeoMesh * neo;
uint32_t last = 0;

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
```

### Change a nodes ID
```
/*
 *  This example changes a NeoCortec nodes node id.
 *  When a modules node id is changed, the new ID is saved in a safe place
 *  and will not be erased by a powercycle. Therefore this code only needs to run once
 *  and then be erased from the sketch. If this is part of an embedded sensoring system
 *  that turns on and is supposed to stay on for months or years,
 *  it does no harm to leave the code in. It will take longer to boot up the system however.
 */

#include <NeoMesh.h>

#define NODE_ID 17
#define CTS_PIN 2

NeoMesh * neo;

void setup()
{
    Serial1.begin(DEFAULT_NEOCORTEC_BAUDRATE);
    neo = new NeoMesh(&Serial1, CTS_PIN);
    neo->change_node_id(NODE_ID);
    neo->start();
}

void loop()
{
    neo->update();

    /* Send or receive messages from NeoMesh */
}
```

### Receive messages
```
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
```