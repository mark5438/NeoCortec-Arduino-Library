#include <NeoMesh.h>
#include <HardwareSerial.h>

#define CTS_GPIO 2
#define NODE_ID 0x0016

NeoMesh * neo;

uint32_t last;

void acknowledged(tNcApiHostAckNack * m)
{
  Serial2.print("Node ");
  Serial2.print(m->originId);
  Serial2.println(" acknowledged a message");
}

void setup()
{
  Serial2.begin(115200);
  Serial1.begin(115200);
  
  pinMode(2, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  neo = new NeoMesh(&Serial1, CTS_GPIO);
  neo->start();
  neo->host_ack_callback = acknowledged;
  // neo->change_node_id(NODE_ID);

  NcSetting nodeid;
  neo->get_setting(NODE_ID_SETTING, &nodeid);

  Serial2.write(nodeid.value, nodeid.length);
  
  last = millis();
}

void loop()
{
  neo->update();

  if (millis() - last > 5000)
  {
    uint8_t test[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    neo->send_acknowledged(0x0010, 0, test, 10);
    last = millis();
  }
}
