# NeoCortec Arduino Library

Communicate over a NeoMesh network using an Arduino an this super user friendly library.

<a href="class_neo_mesh.html">See NeoMesh class</a>

## Example usage

```
#include "neomesh.h"

#define PROTOCOL_UART 0
#define CTS_PIN 2

NeoMesh * neo;

void data_callback(uint8_t n, tNcApiHostData * m)
{
    uint8_t * payload     = m->payload;         // The message
    uint8_t   payload_len = m->payloadLength;   // How long is the message
    uint16_t  sender      = m->originId;        // Who sent the message

    /*
        Handle message
    */
}

void setup()
{
    neo = new NeoMesh(PROTOCOL_UART, CTS_PIN);
    neo.host_data_callback = data_callback;
    neo.start();

    // Send a message using the NeoMesh protocol
    uint8_t data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    neo.send_acknowledged(0x10, 0, data, 10)
}

void loop()
{
    neo->update();
}
```