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

#ifndef NEOPARSER_H_
#define NEOPARSER_H_

#include <stdint.h>
#include "NcApi.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum NcApiMessageType
{
	// Sending
	CommandUnacknowledgedEnum = 0x02,
	CommandAcknowledgedEnum   = 0x03,
	NodeInfoRequestEnum       = 0x08,
	NeighborListRequestEnum   = 0x09,
	NetCmdEnum                = 0x0a,
	RouteInfoRequestEnum	  = 0x0c,
	WesCmdEnum                = 0x10,
    WesResponseEnum           = 0x11,
	AltCmdEnum                = 0x20,

	// Receiving
	HostAckEnum               = 0x50,
	HostNAckEnum              = 0x51,
	HostDataEnum              = 0x52,
	HostDataHapaEnum          = 0x53,
	HostUappDataEnum          = 0x54,
	HostUappDataHapaEnum      = 0x55,
	HostUappDataSend          = 0x56,
	HostUappDataDropped       = 0x57,
	
	NodeInfoReplyEnum         = 0x58,
	NeighborListReplyEnum     = 0x59,
	NetCmdReplyEnum           = 0x5a,
	RouteInfoRequestReplyEnum = 0x5c,
	WesStatusEnum             = 0x60,
	WesSetupRequestEnum       = 0x61,

	// Special
	CommandRawEnum            = 0xff
} NcApiMessageType;

#define NCAPI_HOST_PREFIX_SIZE 2
#define NCAPI_HOSTACK_LENGTH 2
#define NCAPI_HOSTDATA_HEADER_SIZE 5
#define NCAPI_HOSTDATA_MIN_LENGTH (NCAPI_HOSTDATA_HEADER_SIZE+NCAPI_HOST_PREFIX_SIZE)
#define NCAPI_HOSTDATAHAPA_HEADER_SIZE 7
#define NCAPI_HOSTDATAHAPA_MIN_LENGTH (NCAPI_HOSTDATAHAPA_HEADER_SIZE+NCAPI_HOST_PREFIX_SIZE)
#define NCAPI_HOSTUAPPDATA_HEADER_SIZE 7
#define NCAPI_HOSTUAPPDATA_MIN_LENGTH (NCAPI_HOSTUAPPDATA_HEADER_SIZE+NCAPI_HOST_PREFIX_SIZE)
#define NCAPI_HOSTUAPPDATAHAPA_HEADER_SIZE 9
#define NCAPI_HOSTUAPPDATAHAPA_MIN_LENGTH (NCAPI_HOSTUAPPDATAHAPA_HEADER_SIZE+NCAPI_HOST_PREFIX_SIZE)

#define NCAPI_NODEINFOREQUEST_LENGTH 0
#define NCAPI_NEIGHBORLISTREQUEST_LENGTH 0
#define NCAPI_NETCMD_LENGTH unused
#define NCAPI_WESCMD_LENGTH 1
#define NCAPI_WESSETUPREQUEST_LENGTH 6
#define NCAPI_ALTCMD_LENGTH 1

#define NCAPI_NODEINFOREPLY_LENGTH 8
#define NCAPI_ROUTEINFOREQUESTREPLY_LENGTH 16
#define NCAPI_WESRESPONSE_LENGTH (7 + WES_APPSETTINGS_LENGTH)
#define NCAPI_WESSTATUS_LENGTH 1
#define NCAPI_NEIGHBORLISTREPLY_LENGTH 36
#define NCAPI_NEIGHBORLISTREPLY_EX_LENGTH 39
#define NCAPI_NETCMDRESPONSE_HEADER_SIZE 3
#define NCAPI_NETCMDRESPONSE_MIN_LENGTH 5


/**
 * \brief Determines if the content in the buffer is a valid Protocol-message
 * @param buffer Received RX-data
 * @param bufLength Number of received bytes
 * @param[out] outStartAt If a message was found, index into buffer where the message begins
 * @param[out] outLength If a message was found, the message length
 * @return 1==true 0==false
 */
int NcApiIsValidApiFrame(uint8_t * buffer, uint16_t bufLength, uint16_t * outStartAt, uint16_t * outLength);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsHostAck(uint8_t * buffer, tNcApiHostAckNack * p);


/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsHostUappStatus(uint8_t * buffer, tNcApiHostUappStatus * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsHostData(uint8_t * buffer, tNcApiHostData * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsHostDataHapa(uint8_t * buffer, tNcApiHostDataHapa * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsHostUappData(uint8_t * buffer, tNcApiHostUappData * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsHostUappDataHapa(uint8_t * buffer, tNcApiHostUappDataHapa * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsNodeInfoReply(uint8_t * buffer, tNcApiNodeInfoReply * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsWesStatus(uint8_t * buffer, tNcApiWesStatus * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsWesSetupRequest(uint8_t * buffer, tNcApiWesSetupRequest * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsNodeInfo(uint8_t * buffer, tNcApiNodeInfoReply * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsNeighborListReply(uint8_t * buffer, tNcApiNeighborListReply * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsRouteInfoRequestReply(uint8_t * buffer, tNcApiRouteInfoRequestReply * p);

/**
 * \brief Deserializes the content of the buffer into a corresponding structure
 * @param buffer Buffer containing message
 * @param p Instance to deserialize into
 */
void NcApiGetMsgAsNetCmdResponse(uint8_t * buffer, tNcApiNetCmdReply * p);


#ifdef __cplusplus
}
#endif


#endif /* NEOPARSER_H_ */
