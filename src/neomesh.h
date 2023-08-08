/*******************************************************************************
 * @file NeoMesh.h
 * @date 2023-08-03
 * @author Markus Rytter (markus.r@live.dk)
 *
 * @copyright Copyright (c) 2023
 *
 *******************************************************************************/

/**
 * @addtogroup NeoMesh
 * @{
 */

#ifndef NEOMESH_H
#define NEOMESH_H

/*******************************************************************************
 *    Includes
 ******************************************************************************/
#include <Stream.h>
#include <Arduino.h>
#include "NcApi.h"
#include "SAPIParser.h"

/*******************************************************************************
 *    Defines
 ******************************************************************************/

#define DEFAULT_NEOCORTEC_BAUDRATE 115200

#define SAPI_COMMAND_HEAD 0x3E
#define SAPI_COMMAND_TAIL 0x21
#define SAPI_COMMAND_LOGIN1 0x01
#define SAPI_COMMAND_LOGIN2 0x03
#define SAPI_COMMAND_START_BOOTLOADER1 0x01
#define SAPI_COMMAND_START_BOOTLOADER2 0x13
#define SAPI_COMMAND_GET_SETTING_FLASH1 0x01
#define SAPI_COMMAND_GET_SETTING_FLASH2 0x06
#define SAPI_COMMAND_SET_SETTING1 0x01
#define SAPI_COMMAND_SET_SETTING2 0x0A
#define SAPI_COMMAND_COMMIT_SETTINGS1 0x01
#define SAPI_COMMAND_COMMIT_SETTINGS2 0x08
#define SAPI_COMMAND_START_PROTOCOL1 0x01
#define SAPI_COMMAND_START_PROTOCOL2 0x12

#define NODE_ID_SETTING 0xA
#define NETWORK_ID_SETTING 0x2A
#define TRACE_OUTPUT_SETTING 0x2C
#define GENERIC_APPLICATION_NORM_SETTING 0x19
#define GENERIC_APPLICATION_ALT_SETTING 0x3A

#define DEFAULT_PASSWORD_LVL10 {0x4c, 0x76, 0x6c, 0x31, 0x30}

/*******************************************************************************
 *    Class prototypes
 ******************************************************************************/

/**
 * @brief Object that handles connection to NeoCortec module
 */
class NeoMesh
{
public:
    /**
     * @brief Construct new NeoMesh object
     * @param uart_num Which UART is connected to the AAPI UART of the NeoCortec module
     * @param serial Pointer to the Stream object attached to UART
    */
    NeoMesh(Stream * serial, uint8_t cts_pin);

    /**
     * @brief Starts the NeoMesh API
     */
    void start();

    // IGNORE:
    void write(uint8_t *finalMsg, uint8_t finalMsgLength);

    /**
     * @brief Handles all housekeeping. Should be called from main loop
     */
    void update();

    /**
     * @brief Change the id of the node in the NeoMesh network
     * When the ID of a node is changed, it will not revert on reboot.
     * The ID is saved safely within the NeoCortec module.
     * This function reboots the NeoCortec module, so it will not be
     * possible to send data from this node for a period of time after calling this function
     * @param node_id The new nodeid. NOTE: Can not be 0
     */
    void change_node_id(uint16_t node_id);

    /**
     * @brief Change the network id
     * Change the network ID setting within the NeoCortec module.
     * As goes for the node id, the network id is not reverted on reboot.
     * Alle nodes in a network must have the same network id in order to communicate
     * @param network_id The new network id as 16 bytes
     */
    void change_network_id(uint8_t network_id[16]);

    /**
    * @brief Change trace output setting
    * Tracing output enables the user to see neighbors connected to the node in realtime.
    * Of course, this comes with the cost of higher power consumption, which is why you
    * usually only want this setting to be turned on, on gateway nodes with main power.
    * Batterypowered sensor nodes should have this setting turned off.
    * @param mode True if trace output should be turned on
    */
    void change_trace_output_setting(bool mode);

    /**
     * @brief Change baudrate (Must be called before start)
     * If the module is configured to use a dfferent baudrate than 115200,
     * this function must be called with the custom baudrate before the start function is called
     */
    void set_baudrate(uint32_t baudrate);

    /**
     * @brief send an unacknowledged message to a node in the network
     * @param destNodeId The node id of the recepient
     * @param port Which port to send to. Allows recepient to filter messages. If not used, write 0
     * @param appSeqNo message sequence number. If more messages are sent after each other, the sequence number must be different each time
     * @param payload The payload data to send
     * @param payloadLen The length of the payload array
     */
    void send_unacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t *payload, uint8_t payloadLen);

    /**
     * @brief send an acknowledged message to a node in the network
     * If the host_ack_callback is set it will be called when the message recepient has acknowledged
     * @param destNodeId The node id of the recepient
     * @param port Which port to send to. Allows recepient to filter messages. If not used, write 0
     * @param payload The payload data to send
     * @param payloadLen The length of the payload array
     */
    void send_acknowledged(uint16_t destNodeId, uint8_t port, uint8_t *payload, uint8_t payloadLen);

    /**
     * @brief Send a WES command to the node
     * @param cmd The command
     */
    void send_wes_command(NcApiWesCmdValues cmd);

    /**
     * @brief Send a wes response
     * @param uid
     * @param nodeId
     */
    void send_wes_respond(uint64_t uid, uint16_t nodeId);

    void set_debug_serial(Stream * debug_serial);

    // TODO: Create new function definitions that doesn't take parameter n
    // Remember to update samples
    pfnNcApiReadCallback read_callback = 0;
    pfnNcApiHostAckCallback host_ack_callback = 0;
    pfnNcApiHostAckCallback host_nack_callback = 0;
    pfnNcApiHostDataCallback host_data_callback = 0;
    pfnNcApiHostDataHapaCallback host_data_hapa_callback = 0;
    pfnNcApiWesSetupRequestCallback wes_setup_request_callback = 0;
    pfnNcApiWesStatusCallback wes_status_callback = 0;

    // IGNORE:
    static void pass_through_cts0();
    static void pass_through_cts1();
    static void pass_through_cts2();
    static void pass_through_cts3();

private:
    uint8_t uart_num;
    uint8_t cts_pin;
    uint32_t baudrate = DEFAULT_NEOCORTEC_BAUDRATE;

    Stream * serial;
    Stream * debug_serial = 0;

    SAPIParser sapi_parser;

    uint8_t password[5] = DEFAULT_PASSWORD_LVL10; // TODO: Create setter function

    void switch_sapi_aapi();
    void login_sapi(uint8_t * password);
    void change_node_id_sapi(uint16_t nodeid);
    void write_raw(uint8_t *data, uint8_t length);
    bool wait_for_sapi_response(tNcSapiMessage * message, uint32_t timeout_ms);
    void start_bootloader();
    void start_protocol_stack();
    void get_setting(uint8_t setting);  // TODO: Doesn't get anything. Just requests
    void set_setting(uint8_t setting, uint8_t *setting_value, uint8_t setting_value_length);
    void commit_settings();
    void write_sapi_command(uint8_t cmd1, uint8_t cmd2, uint8_t * data, uint8_t data_length);
    void set_password(uint8_t new_password[5]);
    bool change_setting(uint8_t setting, uint8_t * value, uint8_t length);

    void write_debug(const char * str);

    static void read_callback_(uint8_t n, uint8_t *msg, uint8_t msgLength);
    static void host_ack_callback_(uint8_t n, tNcApiHostAckNack *p);
    static void host_nack_callback_(uint8_t n, tNcApiHostAckNack *p);
    static void host_data_callback_(uint8_t n, tNcApiHostData *m);
    static void host_data_hapa_callback_(uint8_t n, tNcApiHostDataHapa *p);
    static void wes_setup_request_callback_(uint8_t n, tNcApiWesSetupRequest *p);
    static void wes_status_callback_(uint8_t n, tNcApiWesStatus *p);
};

/*******************************************************************************/
/** @} addtogroup end */

#endif // NEOMESH_H