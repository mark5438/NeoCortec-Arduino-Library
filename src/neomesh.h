/*******************************************************************************
 * @file neomesh.h
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

#include <HardwareSerial.h>
#include <Arduino.h>
#include "NcApi.h"


/*******************************************************************************
 *    Defines
 ******************************************************************************/

#define DEFAULT_NEOCORTEC_BAUDRATE 115200

#define SAPI_COMMAND_HEAD 0x3E
#define SAPI_COMMAND_TAIL 0x21
#define SAPI_COMMAND_LOGIN1 0x01
#define SAPI_COMMAND_LOGIN2 0x03

#define SAPI_LOGIN_COMMAND_LENGTH 10

#if defined(UBRRH) || defined(UBRR0H)
  extern HardwareSerial Serial;
  #define HAVE_HWSERIAL0
#endif
#if defined(UBRR1H)
  extern HardwareSerial Serial1;
  #define HAVE_HWSERIAL1
#endif
#if defined(UBRR2H)
  extern HardwareSerial Serial2;
  #define HAVE_HWSERIAL2
#endif
#if defined(UBRR3H)
  extern HardwareSerial Serial3;
  #define HAVE_HWSERIAL3
#endif


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
   * @param cts_pin The GPIO connected to the cts pin of the NeoCortec module
   */
  NeoMesh(uint8_t uart_num, uint8_t cts_pin);

  /**
   * @brief Starts the NeoMesh API
   */
  void start();

  // IGNORE:
  void write(uint8_t * finalMsg, uint8_t finalMsgLength);

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
  void send_unacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t * payload, uint8_t payloadLen);
  
  /**
   * @brief send an acknowledged message to a node in the network
   * If the host_ack_callback is set it will be called when the message recepient has acknowledged
   * @param destNodeId The node id of the recepient
   * @param port Which port to send to. Allows recepient to filter messages. If not used, write 0
   * @param payload The payload data to send
   * @param payloadLen The length of the payload array
   */
  void send_acknowledged(uint16_t destNodeId, uint8_t port, uint8_t * payload, uint8_t payloadLen);

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

  pfnNcApiReadCallback read_callback = 0;
  pfnNcApiHostAckCallback host_ack_callback = 0;
  pfnNcApiHostAckCallback host_nack_callback = 0;
  pfnNcApiHostDataCallback host_data_callback = 0;
  pfnNcApiHostDataHapaCallback host_data_hapa_callback = 0;
  pfnNcApiWesSetupRequestCallback wes_setup_request_callback = 0;
  pfnNcApiWesStatusCallback wes_status_callback = 0;

  // IGNORE:
  void message_written();
  static void pass_through_cts0();
  static void pass_through_cts1();
  static void pass_through_cts2();
  static void pass_through_cts3();

private:
  uint8_t uart_num;
  uint8_t cts_pin;
  uint32_t baudrate = DEFAULT_NEOCORTEC_BAUDRATE;
  HardwareSerial * serial;

  bool _message_written = false;

  void wait_for_message_written();
  void switch_sapi_aapi();
  void login_sapi();
  void change_node_id_sapi(uint16_t nodeid);
  void write_raw(uint8_t * data, uint8_t length);

  static pfnNcApiReadCallback read_callback_(uint8_t n, uint8_t * msg, uint8_t msgLength);
  static pfnNcApiHostAckCallback host_ack_callback_(uint8_t n, tNcApiHostAckNack * p);
  static pfnNcApiHostAckCallback host_nack_callback_(uint8_t n, tNcApiHostAckNack * p);
  static pfnNcApiHostDataCallback host_data_callback_(uint8_t n, tNcApiHostData * m);
  static pfnNcApiHostDataHapaCallback host_data_hapa_callback_(uint8_t n, tNcApiHostDataHapa * p);
  static pfnNcApiWesSetupRequestCallback wes_setup_request_callback_(uint8_t n, tNcApiWesSetupRequest * p);
  static pfnNcApiWesStatusCallback wes_status_callback_(uint8_t n, tNcApiWesStatus * p);

};

/*******************************************************************************/
/** @} addtogroup end */

#endif // NEOMESH_H