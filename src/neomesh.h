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
 *    Type defines
 ******************************************************************************/


/**
 * \brief Application provided function that NcApi calls whenever any valid NeocCortec messages 
 * has been received
 *
 * \details This function will deliver a byte array containing the received raw UART frame. <br>
 * It is normally not necessary to register for this callback, as there are other callbacks 
 * which are specific to the various types of application data.
 *
 * @param msg Pointer to the message
 * @param msgLength Message length in bytes
 */
typedef void (*NeoMeshReadCallback)(uint8_t * msg, uint8_t msgLength);

/**
 * \brief Application provided functions that NcApi calls when a <br> 
 * message type "0x50: Acknowledge for previously sent packet" is received, or a <br> 
 * message type "0x51: Non-Acknowledge for previously sent packet" is received.
 *
 * \details The appropriate function is called when a HostAck or HostNAck message 
 * has been received for a previously sent payload package.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshHostAckCallback)(tNcApiHostAckNack * m);



/**
 * \brief Application provided functions that NcApi calls when a <br> 
 * message type "0x56: Uapp packet send.<br> 
 * message type "0x57: Uapp packet was droped.
 *
 * \details The appropriate function is called when a Uapp send or dropped message 
 * has been received for a previously sent payload package.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshHostUappStatusCallback)(tNcApiHostUappStatus * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x52: Host Data" is received.
 *
 * \details The callback is issued when the modules receive payload data, that requires acknowledge, 
 * from another module in the NEOCORTEC mesh network.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshHostDataCallback)(tNcApiHostData * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x53: Host Data HAPA" is received.
 *
 * \details The callback is issued when the modules receive payload data, that requires acknowledge, 
 * from another module in the NEOCORTEC mesh network which has been configured to use the 
 * High Precision Packet Age feature (HAPA). 
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshHostDataHapaCallback)(tNcApiHostDataHapa * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x54: Host Data Unacknowledged" is received.
 *
 * \details The callback is issued when the modules receive payload data, that NOT requires acknowledge, 
 * from another module in the NEOCORTEC mesh network.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param m Strongly typed message
 */
typedef void(*NeoMeshHostUappDataCallback)(tNcApiHostUappData * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x55: Host Data HAPA Unacknowledged" is received.
 *
 * \details The callback is issued when the modules receive payload data, that requires acknowledge, 
 * from another module in the NEOCORTEC mesh network which has been configured to use the 
 * High Precision Packet Age feature (HAPA). 
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param m Strongly typed message
 */
typedef void(*NeoMeshHostUappDataHapaCallback)(tNcApiHostUappDataHapa * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x58: Node Info Reply" is received.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshNodeInfoReplyCallback)(tNcApiNodeInfoReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x59: Neighbor List Reply" is received.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshNeighborListReplyCallback)(tNcApiNeighborListReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x5c: Route Info Request Reply" is received.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshRouteInfoRequestReplyCallback)(tNcApiRouteInfoRequestReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x5a: Network Command Reply" is received.
 *
 * @param m Strongly typed message
 */
typedef void(*NeoMeshNetCmdResponseCallback)(tNcApiNetCmdReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x60: WES Status" is received.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshWesStatusCallback)(tNcApiWesStatus * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x61: WES Setup Request" is received.
 *
 * @param m Strongly typed message
 */
typedef void (*NeoMeshWesSetupRequestCallback)(tNcApiWesSetupRequest * m);




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
    
    /**
    * @brief Change the password the API should use to log into the NC module
    * In order to change settings on the module, it needs to be in bootloader mode
    * and logged in. The standard login password is "Lvl10". This function only needs to
    * be called if the password on the NC module is different from "Lvl10"
    * @param new_password An array of 5 bytes containing the password
    */
    void set_password(uint8_t new_password[5]);


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
    bool change_setting(uint8_t setting, uint8_t * value, uint8_t length);

    NeoMeshReadCallback read_callback = 0;
    NeoMeshHostAckCallback host_ack_callback = 0;
    NeoMeshHostAckCallback host_nack_callback = 0;
    NeoMeshHostDataCallback host_data_callback = 0;
    NeoMeshHostDataHapaCallback host_data_hapa_callback = 0;
    NeoMeshWesSetupRequestCallback wes_setup_request_callback = 0;
    NeoMeshWesStatusCallback wes_status_callback = 0;

    // IGNORE:
    static void pass_through_cts();

private:
    uint8_t uart_num;
    uint8_t cts_pin;
    uint32_t baudrate = DEFAULT_NEOCORTEC_BAUDRATE;
    Stream * serial;
    SAPIParser sapi_parser;

    uint8_t password[5] = DEFAULT_PASSWORD_LVL10; // TODO: Create setter function

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