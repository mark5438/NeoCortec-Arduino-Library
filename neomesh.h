#include <HardwareSerial.h>
#include <Arduino.h>
#include "NcApi.h"

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

class NeoMesh
{
public:
  NeoMesh(uint8_t uart_num, uint8_t cts_pin);
  void start();
  void write(uint8_t * finalMsg, uint8_t finalMsgLength);
  void update();
  void change_node_id(uint16_t node_id);
  void change_network_id(uint8_t network_id[16]);

  void send_unacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t * payload, uint8_t payloadLen);
  void send_acknowledged(uint16_t destNodeId, uint8_t port, uint8_t * payload, uint8_t payloadLen);
  void send_wes_command(NcApiWesCmdValues cmd);
  void send_wes_respond(uint64_t uid, uint16_t nodeId);

  pfnNcApiReadCallback read_callback;
  pfnNcApiHostAckCallback host_ack_callback;
  pfnNcApiHostAckCallback host_nack_callback;
  pfnNcApiHostDataCallback host_data_callback;
  pfnNcApiHostDataHapaCallback host_data_hapa_callback;
  pfnNcApiWesSetupRequestCallback wes_setup_request_callback;
  pfnNcApiWesStatusCallback wes_status_callback;
  
  static void pass_through_cts0();
  static void pass_through_cts1();
  static void pass_through_cts2();
  static void pass_through_cts3();

private:
  uint8_t uart_num;
  uint8_t cts_pin;
  HardwareSerial * serial;

  void switch_sapi_aapi();

  static pfnNcApiReadCallback read_callback_(uint8_t n, uint8_t * msg, uint8_t msgLength);
  static pfnNcApiHostAckCallback host_ack_callback_(uint8_t n, tNcApiHostAckNack * p);
  static pfnNcApiHostAckCallback host_nack_callback_(uint8_t n, tNcApiHostAckNack * p);
  static pfnNcApiHostDataCallback host_data_callback_(uint8_t n, tNcApiHostData * m);
  static pfnNcApiHostDataHapaCallback host_data_hapa_callback_(uint8_t n, tNcApiHostDataHapa * p);
  static pfnNcApiWesSetupRequestCallback wes_setup_request_callback_(uint8_t n, tNcApiWesSetupRequest * p);
  static pfnNcApiWesStatusCallback wes_status_callback_(uint8_t n, tNcApiWesStatus * p);

};
