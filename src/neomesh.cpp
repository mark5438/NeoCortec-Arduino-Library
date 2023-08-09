/*******************************************************************************
 * @file NeoMesh.cpp
 * @date 2023-08-03
 * @author Markus Rytter (markus.r@live.dk)
 *
 * @copyright Copyright (c) 2023
 *
 ******************************************************************************/

/**
 * @addtogroup NeoMesh
 * @{
 */

/*******************************************************************************
 *    Private Includes
 ******************************************************************************/

#include "NeoMesh.h"

#include "SAPIParser.h"


/*******************************************************************************
 *    Private Defines
 ******************************************************************************/

const int number_of_uarts = 1;

NeoMesh *instances[number_of_uarts];

tNcApi g_ncApi[number_of_uarts];
uint8_t g_numberOfNcApis = number_of_uarts;
tNcApiRxHandlers ncRx;

/*******************************************************************************
 *    Public Class/Functions
 ******************************************************************************/

NeoMesh::NeoMesh(Stream * serial, uint8_t cts_pin)
{
    this->uart_num = 0;
    instances[this->uart_num] = this;
    this->serial = serial;

    pinMode(cts_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(cts_pin), NeoMesh::pass_through_cts0, FALLING);
}

void NeoMesh::start()
{
    //this->serial->begin(this->baudrate);

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
    while (this->serial->available())
    {
        char c = this->serial->read();
        NcApiRxData(this->uart_num, c);
        this->sapi_parser.push_char(c);
    }
}

void NeoMesh::set_password(uint8_t new_password[5])
{
    strncpy((char *) this->password, (char *) new_password, 5);
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

void NeoMesh::change_trace_output_setting(bool mode)
{
    this->change_setting(TRACE_OUTPUT_SETTING, (uint8_t*) &mode, 1);
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
    apiStatus = NcApiSendUnacknowledged(this->uart_num, &args);
    if (apiStatus != NCAPI_OK)
    {
        
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
        // TODO: Tell application something went wrong. Maybe map error codes to error strings
    }
}

void NeoMesh::send_wes_command(NcApiWesCmdValues cmd)
{
    tNcApiWesCmdParams args;
    args.msg.cmd = cmd;
    args.callbackToken = &g_ncApi;
    NcApiSendWesCmd(this->uart_num, &args);
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
    NcApiSendWesResponse(this->uart_num, &args);
}

void NeoMesh::set_debug_serial(Stream * debug_serial)
{
    this->debug_serial = debug_serial;
}


/*******************************************************************************
 *    Private Class/Functions
 ******************************************************************************/

bool NeoMesh::change_setting(uint8_t setting, uint8_t * value, uint8_t length)
{
    // TODO: when calling wait_for_sapi_response, return value should be checked
    bool ret = true;
    tNcSapiMessage message;
    this->switch_sapi_aapi();
    bool response = this->wait_for_sapi_response(&message, 250);

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
            ret = false;
        }
        else
        {
            // Unknown error
            ret = false;
        }
    }
    else
    {
        // Error
        ret = false;
    }

    this->start_protocol_stack();
    this->wait_for_sapi_response(&message, 250);    // List get settings is returned
    this->wait_for_sapi_response(&message, 250);
    return ret;
}

void NeoMesh::switch_sapi_aapi()
{
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

void NeoMesh::get_setting(uint8_t setting)
{
    this->write_sapi_command(SAPI_COMMAND_GET_SETTING_FLASH1, SAPI_COMMAND_GET_SETTING_FLASH2, &setting, 1);
}

void NeoMesh::set_setting(uint8_t setting, uint8_t *setting_value, uint8_t setting_value_length)
{
    uint8_t data[setting_value_length + 1];
    data[0] = setting;
    for (int i = 0; i < setting_value_length; i++)
    {
        data[i + 1] = setting_value[i];
    }
    this->write_sapi_command(SAPI_COMMAND_SET_SETTING1, SAPI_COMMAND_SET_SETTING2, data, setting_value_length + 1);
}

void NeoMesh::commit_settings()
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
    NcApiCtsActive(this->uart_num);  // When in bootloader mode CTS is kept constantly low
    
    // For debugging
    delay(1000);
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

bool NeoMesh::wait_for_sapi_response(tNcSapiMessage * message, uint32_t timeout_ms)
{
    // TODO: Return false after timeout
    while(!this->sapi_parser.message_available())
    {
        this->update();
    }
    *message = this->sapi_parser.get_pending_message();
    return true;
}

void NeoMesh::read_callback_(uint8_t n, uint8_t *msg, uint8_t msgLength)
{
    if (instances[n]->read_callback != 0)
        instances[n]->read_callback(n, msg, msgLength);
}

 void NeoMesh::host_ack_callback_(uint8_t n, tNcApiHostAckNack *p)
{
    if (instances[n]->host_ack_callback != 0)
        instances[n]->host_ack_callback(n, p);
}

void NeoMesh::host_nack_callback_(uint8_t n, tNcApiHostAckNack *p)
{
    if (instances[n]->host_nack_callback != 0)
        instances[n]->host_nack_callback(n, p);
}

void NeoMesh::host_data_callback_(uint8_t n, tNcApiHostData *m)
{
    if (instances[n]->host_data_callback != 0)
        instances[n]->host_data_callback(n, m);
}

void NeoMesh::host_data_hapa_callback_(uint8_t n, tNcApiHostDataHapa *p)
{
    if (instances[n]->host_data_hapa_callback != 0)
        instances[n]->host_data_hapa_callback(n, p);
}

void NeoMesh::wes_setup_request_callback_(uint8_t n, tNcApiWesSetupRequest *p)
{
    if (instances[n]->wes_setup_request_callback != 0)
        instances[n]->wes_setup_request_callback(n, p);
}

void NeoMesh::wes_status_callback_(uint8_t n, tNcApiWesStatus *p)
{
    if (instances[n]->wes_status_callback != 0)
        instances[n]->wes_status_callback(n, p);
    return ;
}

void NeoMesh::pass_through_cts0()
{
    NcApiCtsActive(0);
    Serial.println("CTS");
}

void NeoMesh::pass_through_cts1()
{
    NcApiCtsActive(1);
}

void NeoMesh::pass_through_cts2()
{
    NcApiCtsActive(2);
}

void NeoMesh::pass_through_cts3()
{
    NcApiCtsActive(3);
}

NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t *finalMsg, uint8_t finalMsgLength)
{
    instances[n]->write(finalMsg, finalMsgLength);
    return NCAPI_OK;
}

void NcApiSupportMessageReceived(uint8_t n, void *callbackToken, uint8_t *msg, uint8_t msgLength)
{
    NcApiExecuteCallbacks(n, msg, msgLength);
}

void NcApiSupportMessageWritten(uint8_t n, void *callbackToken, uint8_t *finalMsg, uint8_t finalMsgLength)
{
    // NeoMesh *neo = (NeoMesh *)callbackToken;
    // neo->message_written();
    // TODO: Eventually a callback needs to go all the way to the application
}



/*******************************************************************************/

/** @} addtogroup end */