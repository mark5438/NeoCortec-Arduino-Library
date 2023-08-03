#include "neomesh.h"

const int number_of_uarts = 0
#ifdef HAVE_HWSERIAL0
+ 1
#endif
#ifdef HAVE_HWSERIAL1
+ 1
#endif
#ifdef HAVE_HWSERIAL2
+ 1
#endif
#ifdef HAVE_HWSERIAL3
+ 1
#endif
;

NeoMesh * instances[number_of_uarts];

tNcApi g_ncApi[number_of_uarts];
uint8_t g_numberOfNcApis = number_of_uarts;
tNcApiRxHandlers ncRx;


NeoMesh::NeoMesh(uint8_t uart_num, uint8_t cts_pin)
{
  instances[uart_num] = this;
  this->uart_num = uart_num;
  switch(this->uart_num)
  {
    case 0:
      this->serial = &Serial;
      attachInterrupt(digitalPinToInterrupt(cts_pin), NeoMesh::pass_through_cts0, FALLING);
      break;
    case 1:
      this->serial = &Serial1;
      attachInterrupt(digitalPinToInterrupt(cts_pin), NeoMesh::pass_through_cts1, FALLING);
      break;
    case 2:
      this->serial = &Serial2;
      attachInterrupt(digitalPinToInterrupt(cts_pin), NeoMesh::pass_through_cts2, FALLING);
      break;
    case 3:
      this->serial = &Serial3;
      attachInterrupt(digitalPinToInterrupt(cts_pin), NeoMesh::pass_through_cts3, FALLING);
      break;
  }
}

void NeoMesh::start()
{
  this->serial->begin(this->baudrate);
  
  tNcApiRxHandlers * rxHandlers = &ncRx;
  memset( rxHandlers, 0, sizeof(tNcApiRxHandlers));

  rxHandlers->pfnReadCallback            = NeoMesh::read_callback_;
  rxHandlers->pfnHostAckCallback         = NeoMesh::host_ack_callback_;
  rxHandlers->pfnHostNAckCallback        = NeoMesh::host_nack_callback_;
  rxHandlers->pfnHostDataCallback        = NeoMesh::host_data_callback_;
  rxHandlers->pfnHostDataHapaCallback    = NeoMesh::host_data_hapa_callback_;
  rxHandlers->pfnWesSetupRequestCallback = NeoMesh::wes_setup_request_callback_;
  rxHandlers->pfnWesStatusCallback       = NeoMesh::wes_status_callback_;
  
  NcApiInit();
  
  g_ncApi[1].NcApiRxHandlers=&ncRx;
  NcApiCallbackNwuActive(this->uart_num);
}

void NeoMesh::update()
{
  while(this->serial->available())
  {
    char c = this->serial->read();
    char str[8];
    sprintf(str, "0x%x", c);
    Serial.println(str);
    NcApiRxData(this->uart_num, c);
  }
}

void NeoMesh::write(uint8_t * finalMsg, uint8_t finalMsgLength)
{
  this->serial->write(finalMsg, finalMsgLength);
}


void NeoMesh::change_node_id(uint16_t node_id)
{
  this->switch_sapi_aapi();

  // Send login command

  // Change node ID

  //Reboot device
}

void NeoMesh::change_network_id(uint8_t network_id[16])
{
  this->switch_sapi_aapi();

  // Send login command

  // Change network ID

  //Reboot device  
}

void NeoMesh::message_written()
{
  Serial.println("Message written!");
}

void NeoMesh::switch_sapi_aapi()
{
  Serial.println("Switching SAPI to AAPI");

  tNcApiAltCmdMessage cmd_msg = {
    .cmd = 0x0B
  };

  tNcApiAltCmdParams params = {
    .msg = cmd_msg,
    .callbackToken = this
  };
  
  bool s = NcApiSendAltCmd(this->uart_num, &params);
}

void NeoMesh::set_baudrate(uint32_t baudrate)
{
  this->baudrate = baudrate;
}

void NeoMesh::send_unacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t * payload, uint8_t payloadLen)
{
  tNcApiSendUnackParams args;
  NcApiErrorCodes apiStatus;
  args.msg.destNodeId = destNodeId;
  args.msg.destPort = port;
  args.msg.appSeqNo = appSeqNo;
  args.msg.payload = payload;
  args.msg.payloadLength = payloadLen;
  args.callbackToken = &g_ncApi;
  apiStatus = NcApiSendUnacknowledged(0, &args);
  if (apiStatus != NCAPI_OK)
  {
    ; // Application specific
  }
}

void NeoMesh::send_acknowledged(uint16_t destNodeId, uint8_t port, uint8_t * payload, uint8_t payloadLen)
{
  tNcApiSendAckParams args;
  NcApiErrorCodes apiStatus;
  args.msg.destNodeId = destNodeId;
  args.msg.destPort = port;
  args.msg.payload = payload;
  args.msg.payloadLength = payloadLen;
  args.callbackToken = &g_ncApi;
  apiStatus = NcApiSendAcknowledged(1, &args);
  if (apiStatus != NCAPI_OK)
  {
    ; // Application specific
  }
}

void NeoMesh::send_wes_command(NcApiWesCmdValues cmd)
{
  tNcApiWesCmdParams args;
  args.msg.cmd = cmd;
  args.callbackToken = &g_ncApi;
  NcApiSendWesCmd( 0, &args );
}

void NeoMesh::send_wes_respond(uint64_t uid, uint16_t nodeId)
{
  tNcApiWesResponseParams args;
  args.msg.uid[0] = (uid >> 32) & 0xff;
  args.msg.uid[1] = (uid >> 24) & 0xff;
  args.msg.uid[2] = (uid >> 16) & 0xff;
  args.msg.uid[3] = (uid >> 8) & 0xff;
  args.msg.uid[4] = uid & 0xff;
  args.msg.nodeId = nodeId;
  args.callbackToken = &g_ncApi;
  NcApiSendWesResponse( 0, &args );
}

static pfnNcApiReadCallback NeoMesh::read_callback_(uint8_t n, uint8_t * msg, uint8_t msgLength)
{
  if(instances[n]->read_callback)
    instances[n]->read_callback(n, msg, msgLength);
}

static pfnNcApiHostAckCallback NeoMesh::host_ack_callback_(uint8_t n, tNcApiHostAckNack * p)
{
  if(instances[n]->host_ack_callback)
    instances[n]->host_ack_callback(n, p);  
}
  
static pfnNcApiHostAckCallback NeoMesh::host_nack_callback_(uint8_t n, tNcApiHostAckNack * p)
{
  if(instances[n]->host_nack_callback)
    instances[n]->host_nack_callback(n, p);  
}

static pfnNcApiHostDataCallback NeoMesh::host_data_callback_(uint8_t n, tNcApiHostData * m)
{
  if(instances[n]->host_data_callback)
    instances[n]->host_data_callback(n, m);  
}

static pfnNcApiHostDataHapaCallback NeoMesh::host_data_hapa_callback_(uint8_t n, tNcApiHostDataHapa * p)
{
  if(instances[n]->host_data_hapa_callback)
    instances[n]->host_data_hapa_callback(n, p);  
}

static pfnNcApiWesSetupRequestCallback NeoMesh::wes_setup_request_callback_(uint8_t n, tNcApiWesSetupRequest * p)
{
  if(instances[n]->wes_setup_request_callback)
    instances[n]->wes_setup_request_callback(n, p);  
}

static pfnNcApiWesStatusCallback NeoMesh::wes_status_callback_(uint8_t n, tNcApiWesStatus * p)
{
  if(instances[n]->wes_status_callback)
    instances[n]->wes_status_callback(n, p);  
}

static void NeoMesh::pass_through_cts0()
{
  NcApiCtsActive(0);
}

static void NeoMesh::pass_through_cts1()
{
  NcApiCtsActive(1);
}

static void NeoMesh::pass_through_cts2()
{
  NcApiCtsActive(2);
}

static void NeoMesh::pass_through_cts3()
{
  NcApiCtsActive(3);
}

NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t * finalMsg, uint8_t finalMsgLength)
{
  char str[8];
  for(int i = 0; i < finalMsgLength; i++)
  {
    sprintf(str, "0x%x ", finalMsg[i]);
    Serial.print(str);
  }
  Serial.println();
  instances[n]->write(finalMsg, finalMsgLength);
}

void NcApiSupportMessageReceived(uint8_t n,void * callbackToken, uint8_t * msg, uint8_t msgLength)
{
  NcApiExecuteCallbacks(n,msg,msgLength);
}

void NcApiSupportMessageWritten(uint8_t n, void * callbackToken, uint8_t * finalMsg, uint8_t finalMsgLength)
{
  NeoMesh * neo = (NeoMesh*) callbackToken;
  neo->message_written();
}
