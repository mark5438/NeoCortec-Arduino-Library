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

#include <stddef.h>
#include <string.h>
#include "NcApi.h"
#include "NeoParser.h"

#ifndef ASSERT
#ifdef DEBUG
extern void __error__(char *pcFilename, uint32_t ui32Line);
#define ASSERT(expr) do                                                       \
                     {                                                        \
                         if(!(expr))                                          \
                         {                                                    \
                             __error__(__FILE__, __LINE__);                   \
                         }                                                    \
                     }                                                        \
                     while(0)
#else
#define ASSERT(expr)
#endif//DEBUG
#endif//ASSERT

void NcApiInit()
{
	uint8_t i;
	for (i=0; i<g_numberOfNcApis; i++)
		memset( (void*)&g_ncApi[i], 0, sizeof(tNcApi) );
}


tNcApi * NcApiGetInstance(uint8_t n)
{
	ASSERT(n<g_numberOfNcApis);
	return &g_ncApi[n];
}

void NcApiCallbackNwuActive(uint8_t n)
{
	tNcApi * api = NcApiGetInstance(n);
	api->recvBufIsSynced = 1;
	api->rxPosition = 0;
}

void NcApiTxDataDone(uint8_t n)
{
	tNcApi * api = NcApiGetInstance(n);
	uint8_t len = api->txMsgLen;
	uint8_t * buffer;
	void * cbToken;

	buffer = api->txBuffer;
	cbToken = api->writeCallbackToken;
	api->writeCallbackToken = 0;
	api->txMsgLen = 0;
	NcApiSupportMessageWritten( n, cbToken, buffer, len );

}

void NcApiCtsActive(uint8_t n)
{
	tNcApi * api = NcApiGetInstance(n);
	uint8_t len = api->txMsgLen;
	uint8_t * buffer;
	
	// sync receiver
	
	api->recvBufIsSynced = 1;
	api->rxPosition = 0;
	
	if (len==0)
		return;
	buffer = api->txBuffer;
	if (NcApiSupportTxData( n, buffer, len )!=NCAPI_DATA_PENDING)
	{
		NcApiTxDataDone(n);
	}
}


void NcApiShiftBufferDownAndAdjustPosition(tNcApi * api)
{
	uint16_t remaining = api->rxPosition - 1;
	uint16_t iSrc = 1;
	uint16_t i;
	for (i=0; i<remaining; i++)
		api->rxBuffer[i] = api->rxBuffer[ iSrc + i ];
	api->rxPosition = remaining;
}

void NcApiRxData(uint8_t n, uint8_t byte)
{
	uint16_t msgStartAt, msgLength;
	tNcApi * api = NcApiGetInstance(n);
	if (api->recvBufIsSynced==0)
		return;
	if (api->rxPosition >= NCAPI_RXBUFFER_SIZE)
		NcApiShiftBufferDownAndAdjustPosition(api);
	
	api->rxBuffer[ api->rxPosition ] = byte;
	api->rxPosition++;
	
	if (!NcApiIsValidApiFrame(api->rxBuffer, api->rxPosition, &msgStartAt, &msgLength))
		return;
	
	NcApiSupportMessageReceived(n,api->writeCallbackToken, api->rxBuffer + msgStartAt, (uint8_t)(msgLength & 0xff));
	
	api->rxPosition = 0;
}


void NcApiExecuteCallbacks(uint8_t n, uint8_t * msg, uint8_t msgLength)
{
	tNcApiRxHandlers * handlers;
	tNcApi * api = NcApiGetInstance(n);
	handlers = api->NcApiRxHandlers;

	if (handlers==0)
		return;
	
	if (handlers->pfnReadCallback!=0)
		handlers->pfnReadCallback(n, msg, msgLength);

	switch (msg[0])
	{
		case HostAckEnum:
			if (handlers->pfnHostAckCallback!=0)
			{
				tNcApiHostAckNack ack;
				NcApiGetMsgAsHostAck(msg, &ack);
				handlers->pfnHostAckCallback(n, &ack);
			}
			break;
		case HostNAckEnum:
			if (handlers->pfnHostNAckCallback!=0)
			{
				tNcApiHostAckNack ack;
				NcApiGetMsgAsHostAck(msg, &ack);
				handlers->pfnHostNAckCallback(n, &ack);
			}
			break;
		case HostDataEnum:
			if (handlers->pfnHostDataCallback!=0)
			{
				tNcApiHostData data;
				NcApiGetMsgAsHostData(msg, &data);
				handlers->pfnHostDataCallback(n, &data);
			}
			break;
		case HostDataHapaEnum:
			if (handlers->pfnHostDataHapaCallback!=0)
			{
				tNcApiHostDataHapa dataHapa;
				NcApiGetMsgAsHostDataHapa(msg, &dataHapa);
				handlers->pfnHostDataHapaCallback(n, &dataHapa);
			}
			break;
		case HostUappDataEnum:
			if (handlers->pfnHostUappDataCallback != 0)
			{
				tNcApiHostUappData dataUapp;
				NcApiGetMsgAsHostUappData(msg, &dataUapp);
				handlers->pfnHostUappDataCallback(n, &dataUapp);
			}
			break;
		case HostUappDataHapaEnum:
			if (handlers->pfnHostUappDataHapaCallback != 0)
			{
				tNcApiHostUappDataHapa dataUappHapa;
				NcApiGetMsgAsHostUappDataHapa(msg, &dataUappHapa);
				handlers->pfnHostUappDataHapaCallback(n, &dataUappHapa);
			}
			break;
			
		case HostUappDataSend:
			if (handlers->pfnHostUappSendCallback != 0)
			{
				tNcApiHostUappStatus dataUappStatus;
				NcApiGetMsgAsHostUappStatus(msg, &dataUappStatus);
				handlers->pfnHostUappSendCallback(n, &dataUappStatus);
			}
			break;
		case HostUappDataDropped:
			if (handlers->pfnHostUappDropedCallback != 0)
			{
				tNcApiHostUappStatus dataUappStatus;
				NcApiGetMsgAsHostUappStatus(msg, &dataUappStatus);
				handlers->pfnHostUappDropedCallback(n, &dataUappStatus);
			}
			break;
			
			
			
		case NodeInfoReplyEnum:
			if (handlers->pfnNodeInfoReplyCallback!=0)
			{
				tNcApiNodeInfoReply nodeInfo;
				NcApiGetMsgAsNodeInfoReply(msg, &nodeInfo);
				handlers->pfnNodeInfoReplyCallback(n, &nodeInfo);
			}
			break;
		case NeighborListReplyEnum:
			if (handlers->pfnNeighborListReplyCallback!=0)
			{
				tNcApiNeighborListReply NeighborList;
				NcApiGetMsgAsNeighborListReply(msg, &NeighborList);
				handlers->pfnNeighborListReplyCallback(n, &NeighborList);
			}
			break;
		case RouteInfoRequestReplyEnum:
			if (handlers->pfnRouteInfoRequestReplyCallback!=0)
			{
				tNcApiRouteInfoRequestReply RouteInfoRequest;
				NcApiGetMsgAsRouteInfoRequestReply(msg, &RouteInfoRequest);
				handlers->pfnRouteInfoRequestReplyCallback(n, &RouteInfoRequest);
			}
			break;
		case NetCmdReplyEnum:
			if (handlers->pfnNetCmdResponseCallback != 0)
			{
				tNcApiNetCmdReply NetCmdResponse;
				NcApiGetMsgAsNetCmdResponse(msg, &NetCmdResponse);
				handlers->pfnNetCmdResponseCallback(n, &NetCmdResponse);
			}
			break;
		case WesStatusEnum:
			if (handlers->pfnWesStatusCallback!=0)
			{
				tNcApiWesStatus wesStatus;
				NcApiGetMsgAsWesStatus(msg, &wesStatus);
				handlers->pfnWesStatusCallback(n, &wesStatus);
			}
			break;
		case WesSetupRequestEnum:
			if (handlers->pfnWesSetupRequestCallback!=0)
			{
				tNcApiWesSetupRequest wesRequest;
				NcApiGetMsgAsWesSetupRequest(msg, &wesRequest);
				handlers->pfnWesSetupRequestCallback(n, &wesRequest);
			}
			break;
	}
}

NcApiErrorCodes NcApiStatus
(
	uint8_t n
)
{
	tNcApi * api = NcApiGetInstance(n);
	if (api->txMsgLen != 0) return NCAPI_BUSY;

	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendUnacknowledged
(
	uint8_t n,
	tNcApiSendUnackParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
#define PREAMPLE_UNACK 5
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args == 0) return NCAPI_ERR_NOARGS;
	if (args->msg.destNodeId == 0) return NCAPI_ERR_NODEID;
	if (args->msg.destPort>4) return NCAPI_ERR_DESTPORT;
	if (args->msg.payloadLength>NCAPI_MAX_PAYLOAD_LENGTH) return NCAPI_ERR_PAYLOAD;
	if (args->msg.payloadLength != 0 && args->msg.payload == 0) return NCAPI_ERR_NULLPAYLOAD;
	if (api->txMsgLen != 0) return NCAPI_ERR_ENQUEUED;
	buf = api->txBuffer;
	buf[0] = CommandUnacknowledgedEnum;
	buf[1] = PREAMPLE_UNACK + args->msg.payloadLength;
	buf[2] = (args->msg.destNodeId >> 8) & 0xff;
	buf[3] = args->msg.destNodeId & 0xff;
	buf[4] = args->msg.destPort;
	buf[5] = (args->msg.appSeqNo >> 8) & 0x0f; // Only 12 valid bits totally
	buf[6] = args->msg.appSeqNo & 0xff;
	if (args->msg.payloadLength != 0)
		memcpy(buf + 2 + PREAMPLE_UNACK, args->msg.payload, args->msg.payloadLength);
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + PREAMPLE_UNACK + args->msg.payloadLength;
	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendAcknowledged
(
	uint8_t n,
	tNcApiSendAckParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
#define PREAMPLE_ACK 3
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (args->msg.destNodeId==0) return NCAPI_ERR_NODEID;
	if (args->msg.destPort>4) return NCAPI_ERR_DESTPORT;
	if (args->msg.payloadLength>NCAPI_MAX_PAYLOAD_LENGTH) return NCAPI_ERR_PAYLOAD;
	if (args->msg.payloadLength!=0 && args->msg.payload==0) return NCAPI_ERR_NULLPAYLOAD;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;
	buf = api->txBuffer;
	buf[0] = CommandAcknowledgedEnum;
	buf[1] = PREAMPLE_ACK + args->msg.payloadLength;
	buf[2] = (args->msg.destNodeId >> 8) & 0xff;
	buf[3] = args->msg.destNodeId  & 0xff;
	buf[4] = args->msg.destPort;
	if (args->msg.payloadLength!=0)
		memcpy( buf + 2 + PREAMPLE_ACK, args->msg.payload, args->msg.payloadLength );
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + PREAMPLE_ACK + args->msg.payloadLength;
	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendNodeInfoRequest
(
	uint8_t n,
	tNcApiNodeInfoParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

	buf = api->txBuffer;
	buf[0] = NodeInfoRequestEnum;
	buf[1] = NCAPI_NODEINFOREQUEST_LENGTH;
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + NCAPI_NODEINFOREQUEST_LENGTH;
	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendNeighborListRequest
(
	uint8_t n,void * callbackToken
)
{
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

	buf = api->txBuffer;
	buf[0] = NeighborListRequestEnum;
	buf[1] = NCAPI_NEIGHBORLISTREQUEST_LENGTH;
	api->writeCallbackToken = callbackToken;
	api->txMsgLen = 2 + NCAPI_NEIGHBORLISTREQUEST_LENGTH;
	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendRouteInfoRequest
(
	uint8_t n,void * callbackToken
)
{
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

	buf = api->txBuffer;
	buf[0] = RouteInfoRequestEnum;
	buf[1] = 0;
	api->writeCallbackToken = callbackToken;
	api->txMsgLen = 2 + 0;
	return NCAPI_OK;
}


NcApiErrorCodes NcApiSendNetCmd
(
uint8_t n,
tNcApiNetCmdParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t len;
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

	buf = api->txBuffer;
	len = 3 + args->msg.payloadLength;
	buf[0] = NetCmdEnum;
	buf[1] = len;
	buf[2] = (args->msg.destNodeId >> 8) & 0xff;
	buf[3] = args->msg.destNodeId & 0xff;
	buf[4] = args->msg.cmd;

	if (args->msg.payloadLength != 0)
		memcpy(buf + 5, args->msg.payload, args->msg.payloadLength);

	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + len;
	return NCAPI_OK;

}

NcApiErrorCodes NcApiSendWesCmd
(
	uint8_t n,
	tNcApiWesCmdParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

	buf = api->txBuffer;
	buf[0] = WesCmdEnum;
	buf[1] = NCAPI_WESCMD_LENGTH;
	buf[2] = args->msg.cmd;;
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + NCAPI_WESCMD_LENGTH;
	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendWesResponse
(
	uint8_t n,
	tNcApiWesResponseParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t i;
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

 	buf = api->txBuffer;
	buf[0] = WesResponseEnum;
	buf[1] = NCAPI_WESRESPONSE_LENGTH;
	buf[2] = args->msg.uid[0];
	buf[3] = args->msg.uid[1];
	buf[4] = args->msg.uid[2];
	buf[5] = args->msg.uid[3];
	buf[6] = args->msg.uid[4];
	buf[7] = (args->msg.nodeId >> 8) & 0xff;
	buf[8] = args->msg.nodeId  & 0xff;
	for (i=0; i<WES_APPSETTINGS_LENGTH; i++)
	{
		buf[9+i] = args->msg.appSettings[i];
	}
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + NCAPI_WESRESPONSE_LENGTH;
	return NCAPI_OK;
}

NcApiErrorCodes NcApiSendAltCmd
(
	uint8_t n,
	tNcApiAltCmdParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;

	buf = api->txBuffer;
	buf[0] = AltCmdEnum;
	buf[1] = NCAPI_ALTCMD_LENGTH;
	buf[2] = args->msg.cmd;
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = 2 + NCAPI_ALTCMD_LENGTH;
	return NCAPI_OK;
}

void NcApiCancelEnqueuedMessage(uint8_t n)
{
	tNcApi * api = NcApiGetInstance(n);
	api->txMsgLen = 0;
	api->writeCallbackToken = NULL;
}

NcApiErrorCodes NcApiSendRaw
(
	uint8_t n,
	tNcApiSendAckParams * args
)
{
	// Pack the message to be sent taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t * buf;
	tNcApi * api = NcApiGetInstance(n);
	if (args==0) return NCAPI_ERR_NOARGS;
	if (api->txMsgLen!=0) return NCAPI_ERR_ENQUEUED;
	if (args->msg.payloadLength!=0 && args->msg.payload==0) return NCAPI_ERR_NULLPAYLOAD;

	buf = api->txBuffer;

	if (args->msg.payloadLength!=0)
		memcpy( buf, args->msg.payload, args->msg.payloadLength );
	
	api->writeCallbackToken = args->callbackToken;
	api->txMsgLen = args->msg.payloadLength;
	return NCAPI_OK;
}
