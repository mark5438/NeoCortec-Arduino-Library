/*
 Copyright (c) 2015, NeoCortec A/S
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NCAPI_H_
#define NCAPI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCAPI_TXBUFFER_SIZE
#define NCAPI_TXBUFFER_SIZE 32 //!< Default TX buffer size. Can be defined by the application
#define NCAPI_MAX_PAYLOAD_LENGTH (NCAPI_TXBUFFER_SIZE-5)
#endif

#ifndef NCAPI_RXBUFFER_SIZE
#define NCAPI_RXBUFFER_SIZE 255 //!< Default RX buffer size. Can be defined by the application
#endif

#define NCAPI_UNUSED(X) (void)X

typedef enum {
	NCAPI_OK = 0,              	//!< Success
	NCAPI_ERR_NODEID = 1,      	//!< NodeId cannot be 0
	NCAPI_ERR_DESTPORT = 2,    	//!< Port must be [0..3]
	NCAPI_ERR_PAYLOAD = 3,     	//!< No payload supplied
	NCAPI_ERR_ENQUEUED = 4, 	//!< There is already one pending message waiting to be written to the UART
	NCAPI_ERR_NULLPAYLOAD = 5, 	//!< Payload length supplied but no payload
	NCAPI_ERR_NOARGS = 6,      	//!< No arguments pointer
	NCAPI_BUSY = 7,				//!< There is one pending message waiting to be written to the UART
	NCAPI_DATA_PENDING = 8		//!< UART data are still waiting to be send
} NcApiErrorCodes;

typedef enum {
	NCAPI_WES_STOP = 0,             //!< Stop
	NCAPI_WES_STARTSERVER = 1,    	//!< Start server
	NCAPI_WES_REQUESTSTATUS = 2,   	//!< Request status
	NCAPI_WES_STARTCLIENT = 3,   	//!< Start client
} NcApiWesCmdValues;

typedef enum {
	NCAPI_WES_STOPPED = 0,              //!< Stopped
	NCAPI_WES_SERVERRUNNING = 1,    	//!< Server is running
	NCAPI_WES_CLIENTRUNNING = 2,    	//!< Client is running
} NcApiWesStatusValues;

typedef enum {
	NCAPI_NODE_TYPE_NC2400 = 1,         //!< Hardware is NC2400
	NCAPI_NODE_TYPE_NC1000 = 2,         //!< Hardware is NC1000
	NCAPI_NODE_TYPE_NC0400 = 3,         //!< Hardware is NC0400
	
} NcApiNodeType;

typedef enum {
	NCAPI_ALT_STOP = 0,             //!< Stop
	NCAPI_ALT_START = 1,    		//!< Start server
} NcApiAltCmdValues;

typedef enum {
	NCAPI_NetCmd_ACK = 0,			//!< ACK
	NCAPI_NetCmd_NACK = 1,			//!< NACK
	NCAPI_NetCmd_Hibernate = 2,		//!< Hibernate
	NCAPI_NetCmd_Wake = 3,			//!< Wake
	NCAPI_NetCmd_Wes = 5,			//!< WES
} NcApiNetCmdValues;


typedef struct NcApiNeighbor {
	uint16_t nodeId;
	uint8_t RSSI;
} tNcApiNeighbor;


// --------------------------------------------------
// Definitions of messages to be sent 
// and parameters for the related functions
// --------------------------------------------------


/**
 * \brief
 * Definition of message type "0x02: Unacknowledged Packet"
 */
typedef struct NcApiSendUnackMessage {
	uint16_t destNodeId;		//!< Destination node ID
	uint8_t destPort;			//!< Destination port
	uint16_t appSeqNo;			//!< Application sequence number
	uint8_t * payload;			//!< Pointer to payload, if any
	uint8_t payloadLength;		//!< PayloadLength Length of payload
} tNcApiSendUnackMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x02: Unacknowledged Packet"
*/
typedef struct NcApiSendUnackParams {
	tNcApiSendUnackMessage msg;	//!< The actual corresponding message
	void * callbackToken;		//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiSendUnackParams;

/**
 * \brief
 * Definition of message type "0x03: Acknowledged Packet"
 */
typedef struct NcApiSendAckMessage {
	uint16_t destNodeId;		//!< Destination node ID
	uint8_t destPort;			//!< Destination port
	uint8_t payloadLength;		//!< PayloadLength Length of payload
	uint8_t * payload;			//!< Pointer to payload, if any
} tNcApiSendAckMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x03: Acknowledged Packet"
 */
typedef struct NcApiSendAckParams {
	tNcApiSendAckMessage msg;	//!< The actual corresponding message
	void * callbackToken;		//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiSendAckParams;

/**
 * \brief
 * Definition of message type "0x08: Node Info Request"
 */
typedef struct NcApiNodeInfoRequestMessage {
	void * dummy;				//!< (No parameters)
} tNcApiNodeInfoRequestMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x08: Node Info Request"
 */
typedef struct NcApiNodeInfoParams {
	tNcApiNodeInfoRequestMessage msg;	//!< The actual corresponding message
	void * callbackToken;				//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiNodeInfoParams;

/**
 * \brief
 * Definition of message type "0x09: Neighbor List Request"
 */
typedef struct NcApiNeighborListRequestMessage {
	void * dummy;				//!< (No parameters)
} tNcApiNeighborListRequestMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x09: Neighbor List Request"
 */
typedef struct NcApiNeighborListRequestParams {
	tNcApiNeighborListRequestMessage msg;	//!< The actual corresponding message
	void * callbackToken;					//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiNeighborListRequestParams;

/**
 * \brief
 * Definition of message type "0x0a: Network Command"
 */
typedef struct NcApiNetCmdMessage {
	uint16_t destNodeId;		//!< Destination node ID
	uint8_t cmd;				//!< Network Command
	uint8_t * payload;			//!< Pointer to payload, if any
	uint8_t payloadLength;		//!< PayloadLength Length of payload
} tNcApiNetCmdMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x0a: Network Command"
 */
typedef struct NcApiNetCmdParams {
	tNcApiNetCmdMessage msg;	//!< The actual corresponding message
	void * callbackToken;		//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiNetCmdParams;

/**
 * \brief
 * Definition of message type "0x10: WES Command"
 */
typedef struct NcApiWesCmdMessage {
	NcApiWesCmdValues cmd;		//!< WES Command
} tNcApiWesCmdMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x10: WES Command"
 */
typedef struct NcApiWesCmdParams {
	tNcApiWesCmdMessage msg;	//!< The actual corresponding message
	void * callbackToken;		//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiWesCmdParams;

/**
 * \brief
 * Definition of message type "0x11: WES Setup Response"
 */
#define WES_APPSETTINGS_LENGTH 24
typedef struct NcApiWesResponseMessage {
	uint8_t uid[5];									//!< UID
	uint16_t nodeId;								//!< NodeId
	uint8_t appSettings[WES_APPSETTINGS_LENGTH];	//!< appSettings
} tNcApiWesResponseMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x11: WES Setup Response"
 */
typedef struct NcApiWesResponseParams {
	tNcApiWesResponseMessage msg;	//!< The actual corresponding message
	void * callbackToken;			//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiWesResponseParams;

/**
 * \brief
 * Definition of message type "0x20: ALT command"
 */
typedef struct NcApiAltCmdMessage {
	NcApiAltCmdValues cmd;		//!< ALT Command
} tNcApiAltCmdMessage;

/**
 * \brief
 * Parameters for the function handling message type "0x20: ALT command"
 */
typedef struct NcApiAltCmdParams {
	tNcApiAltCmdMessage msg;	//!< The actual corresponding message
	void * callbackToken;		//!< Application provided token / context / tag that it wants to called back with. NcApi does not inspect this parameter, it merely passes it along
} tNcApiAltCmdParams;


// --------------------------------------------------
// Definitions of messages to be received
// --------------------------------------------------

/**
 * \brief
 * Parameters for the function handling message type "0x50: Acknowledge for previously sent packet" <br> 
 * Parameters for the function handling message type "0x51: Non-Acknowledge for previously sent packet"
 */
typedef struct NcApiHostAckNack {
	// Message
	uint16_t originId;
} tNcApiHostAckNack;




/**
 * \brief
 * Parameters for the function handling message type "0x56: Uapp packet send<br> 
 * Parameters for the function handling message type "0x57: Uapp packet droped
 */
typedef struct NcApiHostUappStatus {
	// Message
	uint16_t originId;
	uint16_t appSeqNo;
} tNcApiHostUappStatus;




/**
 * \brief
 * Parameters for the function handling message type "0x52: Host Data"
 */
typedef struct NcApiHostData {
	uint16_t originId;
	uint16_t packageAge;
	uint8_t port;
	uint8_t payloadLength;
	uint8_t * payload;
} tNcApiHostData;

/**
 * \brief
 * Parameters for the function handling message type "0x53: Host Data HAPA"
 */
typedef struct NcApiHostDataHapa {
	uint16_t originId;
	uint32_t packageAge;
	uint8_t port;
	uint8_t payloadLength;
	uint8_t * payload;
} tNcApiHostDataHapa;

/**
 * \brief
 * Parameters for the function handling message type "0x54: Host Data Unacknowledged"
 */
typedef struct NcApiHostUappData {
	uint16_t originId;
	uint16_t packageAge;
	uint8_t port;
	uint16_t appSeqNo;
	uint8_t payloadLength;
	uint8_t * payload;
} tNcApiHostUappData;

/**
 * \brief
 * Parameters for the function handling message type "0x55: Host Data HAPA Unacknowledged"
 */
typedef struct NcApiHostUappDataHapa {
	uint16_t originId;
	uint32_t packageAge;
	uint8_t port;
	uint16_t appSeqNo;
	uint8_t payloadLength;
	uint8_t * payload;
} tNcApiHostUappDataHapa;

/**
 * \brief
 * Parameters for the function handling message type "0x58: Node Info Reply"
 */
typedef struct NcApiNodeInfoReply {
	uint16_t nodeId;				//!< Node ID
	uint8_t uid[5];					//!< Node uid
	NcApiNodeType Type;				//!< Node Hardware Type
} tNcApiNodeInfoReply;

/**
 * \brief
 * Parameters for the function handling message type "0x59: Neighbor List Reply"
 */
typedef struct NcApiNeighborListReply {
	uint8_t NeighborsCount;			//!< Numbers of neighbors
	tNcApiNeighbor Neighbor[12];	//!< Array of neighbors
} tNcApiNeighborListReply;


/**
 * \brief
 * Parameters for the function handling message type "0x5c: Route Info Request Reply"
 */
typedef struct NcApiRouteInfoRequestReply {
	uint8_t Bitmap[16];
} tNcApiRouteInfoRequestReply;

/**
 * \brief
 * Parameters for the function handling message type "0x5a: Network command response"
 */
typedef struct NcApiNetCmdReply {
	uint16_t originId;				//!<
	NcApiNetCmdValues cmd;			//!<
	uint8_t payloadLength;			//!< PayloadLength Length of payload
	uint8_t * payload;				//!< Pointer to payload, if any
} tNcApiNetCmdReply;

/**
 * \brief
 * Parameters for the function handling message type "0x60: WES Status"
 */
typedef struct NcApiWesStatus {
	uint8_t Status; //!< See NcApiWesStatusValues
} tNcApiWesStatus;

/**
 * \brief
 * Parameters for the function handling message type "0x61: WES Setup Request"
 */
typedef struct NcApiWesSetupRequest {
	uint8_t uid[5];
	uint8_t appFuncType;
} tNcApiWesSetupRequest;


// --------------------------------------------------
// Definitions of API functions
// --------------------------------------------------

/**
 * \brief Status for sending new command
 *
 * \details This function will return NCAPI_OK or NCAPI_BUSY
 *
 * @param n Index of tNcApi instance that message was written to
 */

NcApiErrorCodes NcApiStatus
(
	uint8_t n
);

/**
 * \brief Application provided function that NcApi calls after it has succesfully received a full message
 *
 * \details The API will call this function once a message has successfully been received in full 
 * from the module. This can be used by the application layer to initiate invocation of the
 * relevant callback function via the function NcApiExecuteCallbacks(). 
 *
 * @param n Index of tNcApi instance that message was written to
 * @param callbackToken Application provided context / token / tag
 * @param msg Pointer to the message
 * @param msgLength Message length in bytes
 */
void NcApiSupportMessageReceived(uint8_t n,void * callbackToken, uint8_t * msg, uint8_t msgLength);

/**
 * \brief Callback from the application into NcApi whenever nWU becomes active
 * @param n Index of tNcApi instance that the nWU interrupt relates to
 */
void NcApiCallbackNwuActive(uint8_t n);

/**
 * \brief Callback from the application into NcApi whenever CTS becomes active
 *
 * \details This is a function inside the API, which shall be called each time the CTS line 
 * on the UART transitions to active (low) state. As such, a monitor for the particular pin 
 * on the controller must be present, such that the API can be made aware that the CTS is active. 
 * There is no need for any action when the CTS line transitions to passive (high).
 *
 * @param n Index of tNcApi instance that the CTS interrupt relates to
 */
void NcApiCtsActive(uint8_t n);

/**
 * \brief Application provied function that NcApi calls if there is any pending data to be 
 * written to the UART
 *
 * \details The API will call this function to send data to the UART. The function is called 
 * with a pointer to the actual data to be written. The function shall implement the necessary 
 * code required to output the data to the UART. If not all data is send whem returning from
 * the function, NCAPI_DATA_PENDING must be returned, and NcApiTxDataDone must be called when 
 * last data is send.
 *
 * @param n Index of tNcApi instance that the data should be written to, ie. which UART
 * @param finalMsg Pointer to the buffer
 * @param finalMsgLength Number of bytes to be written
 * @return NCAPI_OK if all data is send, if any pending data NCAPI_DATA_PENDING is returned.
 */
 NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t * finalMsg, uint8_t finalMsgLength);

/**
 * \brief Callback from the application into NcApi whenever the last data is send over UART
 *
 * \details This is a function inside the API, which shall be called if NcApiSupportTxData
 * returnd NCAPI_DATA_PENDING. The API uses reset internal pointers.
 *
 * @param n Index of tNcApi instance that the CTS interrupt relates to
 */
void NcApiTxDataDone(uint8_t n);

/**
 * \brief Application provided function that NcApi calls after it has succesfully written the message
 *
 * \details The API will call this function once a message has successfully been written in full 
 * to the module. This can be used by the application layer to check that a previous request to 
 * send a message was completed successfully. 
 *
 * @param n Index of tNcApi instance that message was written to
 * @param callbackToken Application provided context / token / tag
 * @param finalMsg Pointer to the message
 * @param finalMsgLength Message length in bytes
 */
void NcApiSupportMessageWritten(uint8_t n, void * callbackToken, uint8_t * finalMsg, uint8_t finalMsgLength);

/**
 * \brief Callback from the application into NcApi whenever a byte is received on the UART
 *
 * \details This is a function inside the API, which shall be called each time a byte is received 
 * on the UART. The byte shall be included as an argument when the function is called.
 *
 * @param n Index of tNcApi instance that the byte relates to
 * @param byte The byte received
 */
void NcApiRxData(uint8_t n, uint8_t byte);

/**
 * \brief Application provided function that NcApi calls whenever any valid NeocCortec messages 
 * has been received
 *
 * \details This function will deliver a byte array containing the received raw UART frame. <br>
 * It is normally not necessary to register for this callback, as there are other callbacks 
 * which are specific to the various types of application data.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param msg Pointer to the message
 * @param msgLength Message length in bytes
 */
typedef void (*pfnNcApiReadCallback)(uint8_t n, uint8_t * msg, uint8_t msgLength);

/**
 * \brief Application provided functions that NcApi calls when a <br> 
 * message type "0x50: Acknowledge for previously sent packet" is received, or a <br> 
 * message type "0x51: Non-Acknowledge for previously sent packet" is received.
 *
 * \details The appropriate function is called when a HostAck or HostNAck message 
 * has been received for a previously sent payload package.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiHostAckCallback)(uint8_t n, tNcApiHostAckNack * m);



/**
 * \brief Application provided functions that NcApi calls when a <br> 
 * message type "0x56: Uapp packet send.<br> 
 * message type "0x57: Uapp packet was droped.
 *
 * \details The appropriate function is called when a Uapp send or dropped message 
 * has been received for a previously sent payload package.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiHostUappStatusCallback)(uint8_t n, tNcApiHostUappStatus * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x52: Host Data" is received.
 *
 * \details The callback is issued when the modules receive payload data, that requires acknowledge, 
 * from another module in the NEOCORTEC mesh network.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiHostDataCallback)(uint8_t n, tNcApiHostData * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x53: Host Data HAPA" is received.
 *
 * \details The callback is issued when the modules receive payload data, that requires acknowledge, 
 * from another module in the NEOCORTEC mesh network which has been configured to use the 
 * High Precision Packet Age feature (HAPA). 
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiHostDataHapaCallback)(uint8_t n, tNcApiHostDataHapa * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x54: Host Data Unacknowledged" is received.
 *
 * \details The callback is issued when the modules receive payload data, that NOT requires acknowledge, 
 * from another module in the NEOCORTEC mesh network.
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void(*pfnNcApiHostUappDataCallback)(uint8_t n, tNcApiHostUappData * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x55: Host Data HAPA Unacknowledged" is received.
 *
 * \details The callback is issued when the modules receive payload data, that requires acknowledge, 
 * from another module in the NEOCORTEC mesh network which has been configured to use the 
 * High Precision Packet Age feature (HAPA). 
 * The callback function delivers a pointer to a struct containing the relevant information.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void(*pfnNcApiHostUappDataHapaCallback)(uint8_t n, tNcApiHostUappDataHapa * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x58: Node Info Reply" is received.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiNodeInfoReplyCallback)(uint8_t n, tNcApiNodeInfoReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x59: Neighbor List Reply" is received.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiNeighborListReplyCallback)(uint8_t n, tNcApiNeighborListReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x5c: Route Info Request Reply" is received.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiRouteInfoRequestReplyCallback)(uint8_t n, tNcApiRouteInfoRequestReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x5a: Network Command Reply" is received.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void(*pfnNcApiNetCmdResponseCallback)(uint8_t n, tNcApiNetCmdReply * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x60: WES Status" is received.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiWesStatusCallback)(uint8_t n, tNcApiWesStatus * m);

/**
 * \brief Application provided function that NcApi calls when a <br> 
 * message type "0x61: WES Setup Request" is received.
 *
 * @param n Index of tNcApi instance that the message was received from
 * @param m Strongly typed message
 */
typedef void (*pfnNcApiWesSetupRequestCallback)(uint8_t n, tNcApiWesSetupRequest * m);


/**
 * \brief Set of application callbacks to handle any received messages. 
 * Each callback is optional allowing the application to register specific 
 * callbacks only for the message types of particular interest.
*/
typedef struct NcApiRxHandlers {
	pfnNcApiReadCallback pfnReadCallback;									//!< Optional callback for all received messages as a byte array
	pfnNcApiHostAckCallback pfnHostAckCallback;								//!< Optional callback for all received HostAck messages
	pfnNcApiHostAckCallback pfnHostNAckCallback;							//!< Optional callback for all received HostNAck messages
	pfnNcApiHostUappStatusCallback pfnHostUappSendCallback;					//!< Optional callback for all received UappSend messages
	pfnNcApiHostUappStatusCallback pfnHostUappDropedCallback;				//!< Optional callback for all received UappDropped messages
	pfnNcApiHostDataCallback pfnHostDataCallback;							//!< Optional callback for all received HostData messages
	pfnNcApiHostDataHapaCallback pfnHostDataHapaCallback;					//!< Optional callback for all received HostDataHapa messages
	pfnNcApiHostUappDataCallback pfnHostUappDataCallback;					//!< Optional callback for all received HostUappData messages
	pfnNcApiHostUappDataHapaCallback pfnHostUappDataHapaCallback;			//!< Optional callback for all received HostUappDataHapa messages
	pfnNcApiNodeInfoReplyCallback pfnNodeInfoReplyCallback;					//!< Optional callback for all received NodeInfoReply messages
	pfnNcApiNeighborListReplyCallback pfnNeighborListReplyCallback;			//!< Optional callback for all received NeighborListReply messages
	pfnNcApiRouteInfoRequestReplyCallback pfnRouteInfoRequestReplyCallback; //!< Optional callback for all received RouteInfoRequestReply messages
	pfnNcApiNetCmdResponseCallback pfnNetCmdResponseCallback;				//!< Optional callback for all received NetCmdResponse messages
	pfnNcApiWesSetupRequestCallback pfnWesSetupRequestCallback;				//!< Optional callback for all received WesSetupRequest messages
	pfnNcApiWesStatusCallback pfnWesStatusCallback;							//!< Optional callback for all received WesStatus messages
} tNcApiRxHandlers;


/**
* \brief API provided function that initializes the allocated instances of tNcApi
*
* \remark This function must be called once to initialize the API.
*/
void NcApiInit(void);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x02: Unacknowledged Packet" shall be sent.
 *
 * \details This function can be used to send payload data to another node inside the mesh network.
 * The message will not be acknowledged.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
*/
NcApiErrorCodes NcApiSendUnacknowledged(uint8_t n, tNcApiSendUnackParams * args);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x03: Acknowledged Packet" shall be sent.
 *
 * \details This function can be used to send payload data to another node inside the mesh network.
 * The message will be acknowledged.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendAcknowledged(uint8_t n, tNcApiSendAckParams * args);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x08: Node Info Request" shall be sent.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendNodeInfoRequest(uint8_t n, tNcApiNodeInfoParams * args);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x09: Neighbor List Request" shall be sent.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendNeighborListRequest(uint8_t n,void * callbackToken);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x0c: Route Info Request" shall be sent.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendRouteInfoRequest(	uint8_t n,void * callbackToken);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x0a: Network Command" shall be sent.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendNetCmd(uint8_t n, tNcApiNetCmdParams * args);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x10: WES Command" shall be sent.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendWesCmd(uint8_t n, tNcApiWesCmdParams * args);

/**
 * \brief API provided function that may be called when a <br> 
 * message type "0x11: WES Setup Response" shall be sent.
 *
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendWesResponse(uint8_t n, tNcApiWesResponseParams * args);

/**
 * \brief Sends one AltCmd message
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendAltCmd(uint8_t n, tNcApiAltCmdParams * args);

/**
 * \brief API provided function that may be called to the relevant callback functions 
 * for a received message
 *
 * \details The Application typically calls this function from NcApiSupportMessageReceived() once a 
 * message has successfully been received from the module, and the  relevant callback function 
 * in the Application vill then be called, if it has previously been registered. 
 *
 * @param n Index of tNcApi instance that message was written to
 * @param msg Pointer to the message
 * @param msgLength Message length in bytes
 */
void NcApiExecuteCallbacks(uint8_t n, uint8_t * msg, uint8_t msgLength);

/**
 * \brief Cancels any enqueued message
 * @param n Index of tNcApi instance where the message should be dequeued
 **/
void NcApiCancelEnqueuedMessage(uint8_t n);


/**
 * \brief (This function is not supported)
 */
/* (Since the function is not supported, the description is excluded from Doxygen)
 *
 * \brief Sends one unknown message
 * @param n Index of tNcApi instance that the message should be sent via
 * @param args Pointer to instance of the argument structure that holds the parameters
 * @return 0 upon success. Anything else is an error
 */
NcApiErrorCodes NcApiSendRaw(uint8_t n,	tNcApiSendAckParams * args);


/**
 * \brief This is the definition of a global structure holding various information, 
 *  in particular the RX and TX buffers for a specific UART, and tha set of application 
 *  callbacks to handle any received messages. These data are managed by the NcApi module, 
 *  and as such the fields are considered internal to NcApi.
 */
typedef struct NcApi {
	uint8_t rxBuffer[ NCAPI_RXBUFFER_SIZE]; //!< Internal UART receive buffer
	uint16_t rxPosition;					//!< Internal position in UART receive buffer
	volatile uint8_t txMsgLen;				//!< Internal UART transmit buffer length
	uint8_t txBuffer[ NCAPI_TXBUFFER_SIZE];	//!< Internal UART transmit buffer
	void * writeCallbackToken;				//!< Internal UART transmit callback token
	volatile uint8_t recvBufIsSynced;		//!< Internal UART receive buffer in sync
	tNcApiRxHandlers * NcApiRxHandlers;     //!< Set of application callbacks to handle any received messages
} tNcApi;

//! \brief Application defined array of NcApi instances in use
extern tNcApi g_ncApi[];

//! \brief Application defined number of elements in the g_ncApi array
extern uint8_t g_numberOfNcApis;


#ifdef __cplusplus
}
#endif

#endif /* NCAPI_H_ */
