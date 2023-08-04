#include <NeoMesh.h>

#define PROTOCOL_UART 0
#define CTS_GPIO 2
#define NODE_ID 0x11

NeoMesh * neo;

uint32_t last;
uint8_t test[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

void setup()
{
  neo = new NeoMesh(PROTOCOL_UART, CTS_GPIO);
  neo->start();
  neo->change_node_id(0x21);
  last = millis();
}

void loop()
{
  neo->update();

  if(millis() - last > 5000)
  {
    neo->send_acknowledged(0x0010, 0, test, 10);
    last = millis();
  }
}
