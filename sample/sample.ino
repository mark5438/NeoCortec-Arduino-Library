#include <neomesh.h>

#define PROTOCOL_UART 1
#define CTS_GPIO 2
#define NODE_ID 0x11

NeoMesh * neo;

void setup()
{
  Serial.begin(115200);
  Serial.println("Started");
  neo = new NeoMesh(PROTOCOL_UART, CTS_GPIO);
  neo->start();
  neo->change_node_id(0x21);
}

void loop()
{
  neo->update();
}
