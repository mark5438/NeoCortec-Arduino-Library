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