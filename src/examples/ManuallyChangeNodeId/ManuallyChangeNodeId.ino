/*
 *  This example changes a NeoCortec nodes node id.
 *  When a modules node id is changed, the new ID is saved in a safe place
 *  and will not be erased by a powercycle. Therefore this code only needs to run once
 *  and then be erased from the sketch. If this is part of an embedded sensoring system
 *  that turns on and is supposed to stay on for months or years,
 *  it does no harm to leave the code in. It will take longer to boot up the system however.
 *
 *  This is a more advanced way of changing a NeoMesh setting. The library can do all of this for you.
 *  See example ChangeNodeId
 */

#include <NeoMesh.h>

#define CTS_PIN 2

NeoMesh * neo;

void setup()
{
    Serial1.begin(DEFAULT_NEOCORTEC_BAUDRATE);
    neo = new NeoMesh(&Serial1, CTS_PIN);
    neo->start();
    


    /*
    *   CHANGE NODE ID:
    */

    uint16_t node_id = 17;

    uint8_t node_id_setting_value[2] = {
        node_id >> 8,
        node_id
    };
    
    tNcSapiMessage message;
    if(neo->get_module_mode() != AAPI || neo->switch_sapi_aapi())
    {
        if(neo->get_module_mode() != SAPI_LOGGED_OUT || neo->login_sapi())
        {
            neo->set_setting(NODE_ID_SETTING, node_id_setting_value, 2);
            neo->wait_for_sapi_response(&message, 250);
            neo->commit_settings();
            neo->wait_for_sapi_response(&message, 250);
        }
        else
        {
            // Login error
        }
    }
    else
    {
        // Error starting bootloader mode
    }

    neo->start_protocol_stack();
    neo->wait_for_sapi_response(&message, 250);    // List get settings is returned
    neo->wait_for_sapi_response(&message, 250);
}

void loop()
{
    neo->update();

    /* Send or receive messages from NeoMesh */
}
