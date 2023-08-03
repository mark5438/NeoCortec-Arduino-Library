#include <neomesh.h>
#include <SAPIParser.h>

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
  Serial.println("Exiting setup");
}

void loop()
{
  neo->update();

  uint8_t test[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  Serial.println("TX");
  neo->send_acknowledged(0x0010, 0, test, 10);

  delay(5000);
}
