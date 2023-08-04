#include "neomesh.h"

#include "SAPIParser.h"

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

NeoMesh *instances[number_of_uarts];

tNcApi g_ncApi[number_of_uarts];
uint8_t g_numberOfNcApis = number_of_uarts;
tNcApiRxHandlers ncRx;

NeoMesh::NeoMesh(uint8_t uart_num, uint8_t cts_pin)
{
    instances[uart_num] = this;
    this->uart_num = uart_num;
    switch (this->uart_num)
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
    this->sapi_parser = new SAPIParser(this->uart_num);

    tNcApiRxHandlers *rxHandlers = &ncRx;
    memset(rxHandlers, 0, sizeof(tNcApiRxHandlers));

    rxHandlers->pfnReadCallback = NeoMesh::read_callback_;
    rxHandlers->pfnHostAckCallback = NeoMesh::host_ack_callback_;
    rxHandlers->pfnHostNAckCallback = NeoMesh::host_nack_callback_;
    rxHandlers->pfnHostDataCallback = NeoMesh::host_data_callback_;
    rxHandlers->pfnHostDataHapaCallback = NeoMesh::host_data_hapa_callback_;
    rxHandlers->pfnWesSetupRequestCallback = NeoMesh::wes_setup_request_callback_;
    rxHandlers->pfnWesStatusCallback = NeoMesh::wes_status_callback_;

    NcApiInit();

    g_ncApi[this->uart_num].NcApiRxHandlers = &ncRx;
    NcApiCallbackNwuActive(this->uart_num);
}

void NeoMesh::update()
{
    bool avb = this->serial->available();
    if(avb)
        Serial.print("<--- ");
    while (this->serial->available())
    {
        char c = this->serial->read();
        char str[32];
        sprintf(str, "0x%X ", c);
        Serial.print(str);
        NcApiRxData(this->uart_num, c);
        this->sapi_parser->push_char(c);
    }
    if(avb)
        Serial.println();
}

void NeoMesh::set_password(uint8_t new_password[5])
{
    strncpy(this->password, new_password, 5);
}

void NeoMesh::write(uint8_t *finalMsg, uint8_t finalMsgLength)
{
    this->serial->write(finalMsg, finalMsgLength);
}

void NeoMesh::change_node_id(uint16_t node_id)
{
    uint8_t new_node_id[2] = {
        node_id >> 8,
        node_id
    };
    this->change_setting(NODE_ID_SETTING, new_node_id, 2);
}

void NeoMesh::change_network_id(uint8_t network_id[16])
{
    this->change_setting(NETWORK_ID_SETTING, network_id, 16);
}

void NeoMesh::change_setting(uint8_t setting, uint8_t * value, uint8_t length)
{
    tNcSapiMessage message;
    this->switch_sapi_aapi();
    bool response = this->wait_for_sapi_response(&message, 250);
    if(!response)
        Serial.println("Timeout");

    if(response && message.command == BootloaderStarted)
    {
        this->login_sapi(this->password);
        response = this->wait_for_sapi_response(&message, 250);
        if(response && message.command == LoginOK)
        {
            this->set_setting(setting, value, length);
            this->wait_for_sapi_response(&message, 250);
            this->commit_settings();
            this->wait_for_sapi_response(&message, 250);
        }
        else if(response && message.command == LoginError)
        {
            // Error logging in
            Serial.println("Error1");
        }
        else
        {
            // Unknown error
            Serial.println("Error2");
        }
    }
    else
    {
        // Error
        Serial.println("Error3");
    }

    this->start_protocol_stack();
    this->wait_for_sapi_response(&message, 250);
    this->wait_for_sapi_response(&message, 250);
}

void NeoMesh::message_written()
{
    this->_message_written = true;
}

void NeoMesh::switch_sapi_aapi()
{
    Serial.println("Switching SAPI to AAPI");

    uint8_t cmd = 0x0B;
    this->write_raw(&cmd, 1);
}

void NeoMesh::login_sapi(uint8_t * password)
{
    this->write_sapi_command(SAPI_COMMAND_LOGIN1, SAPI_COMMAND_LOGIN2, password, 5);
}

void NeoMesh::start_bootloader()
{
    this->write_sapi_command(SAPI_COMMAND_START_BOOTLOADER1, SAPI_COMMAND_START_BOOTLOADER2, nullptr, 0);
}

void NeoMesh::start_protocol_stack()
{
    this->write_sapi_command(SAPI_COMMAND_START_PROTOCOL1, SAPI_COMMAND_START_PROTOCOL2, nullptr, 0);
}

uint8_t NeoMesh::get_setting(uint8_t setting)
{
    this->write_sapi_command(SAPI_COMMAND_GET_SETTING_FLASH1, SAPI_COMMAND_GET_SETTING_FLASH2, &setting, 1);
}

uint8_t NeoMesh::set_setting(uint8_t setting, uint8_t *setting_value, uint8_t setting_value_length)
{
    uint8_t data[setting_value_length + 1];
    data[0] = setting;
    for (int i = 0; i < setting_value_length; i++)
    {
        data[i + 1] = setting_value[i];
    }
    this->write_sapi_command(SAPI_COMMAND_SET_SETTING1, SAPI_COMMAND_SET_SETTING2, data, setting_value_length + 1);
}

uint8_t NeoMesh::commit_settings()
{
    this->write_sapi_command(SAPI_COMMAND_COMMIT_SETTINGS1, SAPI_COMMAND_COMMIT_SETTINGS2, nullptr, 0);
}

void NeoMesh::write_sapi_command(uint8_t cmd1, uint8_t cmd2, uint8_t * data, uint8_t data_length)
{
    uint8_t cmd[5 + data_length] = {
        SAPI_COMMAND_HEAD,
        3 + data_length,
        cmd1,
        cmd2,
    };
    for(int i = 0; i < data_length; i++)
    {
        cmd[4 + i] = data[i];
    }
    cmd[4 + data_length] = SAPI_COMMAND_TAIL;

    this->write_raw(cmd, 5 + data_length);
    NcApiCtsActive(this->uart_num); // TODO: better    
}

void NeoMesh::write_raw(uint8_t *data, uint8_t length)
{
    tNcApiSendAckMessage msg = {
        .destNodeId = 0,
        .destPort = 0,
        .payloadLength = length,
        .payload = data};

    tNcApiSendAckParams params = {
        .msg = msg,
        .callbackToken = this};

    NcApiSendRaw(this->uart_num, &params);
}

bool NeoMesh::wait_for_sapi_response(tNcSapiMessage * message, uint16_t timeout_ms)
{
    // TODO: Return false after timeout
    while(!this->sapi_parser->message_available())  // TODO: Create for function to wait for message with timeout
    {
        this->update();
    }
    *message = this->sapi_parser->get_pending_message();
    return true;
}

void NeoMesh::set_baudrate(uint32_t baudrate)
{
    this->baudrate = baudrate;
}

void NeoMesh::send_unacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t *payload, uint8_t payloadLen)
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

void NeoMesh::send_acknowledged(uint16_t destNodeId, uint8_t port, uint8_t *payload, uint8_t payloadLen)
{
    tNcApiSendAckParams args;
    NcApiErrorCodes apiStatus;
    args.msg.destNodeId = destNodeId;
    args.msg.destPort = port;
    args.msg.payload = payload;
    args.msg.payloadLength = payloadLen;
    args.callbackToken = &g_ncApi;
    apiStatus = NcApiSendAcknowledged(this->uart_num, &args);
    if (apiStatus != NCAPI_OK)
    {
        Serial.print("Error sending acknowledged message: ");
        Serial.println(apiStatus);
    }
}

void NeoMesh::send_wes_command(NcApiWesCmdValues cmd)
{
    tNcApiWesCmdParams args;
    args.msg.cmd = cmd;
    args.callbackToken = &g_ncApi;
    NcApiSendWesCmd(0, &args);
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
    NcApiSendWesResponse(0, &args);
}

static pfnNcApiReadCallback NeoMesh::read_callback_(uint8_t n, uint8_t *msg, uint8_t msgLength)
{
    if (instances[n]->read_callback != 0)
        instances[n]->read_callback(n, msg, msgLength);
}

static pfnNcApiHostAckCallback NeoMesh::host_ack_callback_(uint8_t n, tNcApiHostAckNack *p)
{
    if (instances[n]->host_ack_callback != 0)
        instances[n]->host_ack_callback(n, p);
}

static pfnNcApiHostAckCallback NeoMesh::host_nack_callback_(uint8_t n, tNcApiHostAckNack *p)
{
    if (instances[n]->host_nack_callback != 0)
        instances[n]->host_nack_callback(n, p);
}

static pfnNcApiHostDataCallback NeoMesh::host_data_callback_(uint8_t n, tNcApiHostData *m)
{
    if (instances[n]->host_data_callback != 0)
        instances[n]->host_data_callback(n, m);
}

static pfnNcApiHostDataHapaCallback NeoMesh::host_data_hapa_callback_(uint8_t n, tNcApiHostDataHapa *p)
{
    if (instances[n]->host_data_hapa_callback != 0)
        instances[n]->host_data_hapa_callback(n, p);
}

static pfnNcApiWesSetupRequestCallback NeoMesh::wes_setup_request_callback_(uint8_t n, tNcApiWesSetupRequest *p)
{
    if (instances[n]->wes_setup_request_callback != 0)
        instances[n]->wes_setup_request_callback(n, p);
}

static pfnNcApiWesStatusCallback NeoMesh::wes_status_callback_(uint8_t n, tNcApiWesStatus *p)
{
    if (instances[n]->wes_status_callback != 0)
        instances[n]->wes_status_callback(n, p);
}

static void NeoMesh::pass_through_cts0()
{
    NcApiCtsActive(0);
}

static void NeoMesh::pass_through_cts1()
{
    NcApiCtsActive(1);
    Serial.println("CTS");
}

static void NeoMesh::pass_through_cts2()
{
    NcApiCtsActive(2);
}

static void NeoMesh::pass_through_cts3()
{
    NcApiCtsActive(3);
}

NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t *finalMsg, uint8_t finalMsgLength)
{
    instances[n]->write(finalMsg, finalMsgLength);
    Serial.print("---> ");
    for(int i = 0; i < finalMsgLength; i++)
    {
        char str[32];
        sprintf(str, "0x%X ", finalMsg[i]);
        Serial.print(str);
    }
    Serial.println();
}

void NcApiSupportMessageReceived(uint8_t n, void *callbackToken, uint8_t *msg, uint8_t msgLength)
{
    NcApiExecuteCallbacks(n, msg, msgLength);
}

void NcApiSupportMessageWritten(uint8_t n, void *callbackToken, uint8_t *finalMsg, uint8_t finalMsgLength)
{
    NeoMesh *neo = (NeoMesh *)callbackToken;
    neo->message_written();
}