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

#include "NeoParser.h"

int NcApiIsValidFrameProtocol(uint8_t * buffer, uint16_t bufLength, uint16_t * outLength)
{
	uint8_t len;
	uint8_t apiMsgType;
	if (bufLength<2)
		return 0;
	apiMsgType = buffer[0];
	len = buffer[1];
	if (len==0
		|| (apiMsgType==HostAckEnum && len!=NCAPI_HOSTACK_LENGTH)
		|| (apiMsgType==HostNAckEnum && len!=NCAPI_HOSTACK_LENGTH)
		|| (apiMsgType==HostDataEnum && len<=NCAPI_HOSTDATA_HEADER_SIZE)
		|| (apiMsgType==HostDataHapaEnum && len<=NCAPI_HOSTDATAHAPA_HEADER_SIZE)
		|| (apiMsgType==HostUappDataEnum && len <= NCAPI_HOSTUAPPDATA_HEADER_SIZE)
		|| (apiMsgType==HostUappDataHapaEnum && len <= NCAPI_HOSTUAPPDATAHAPA_HEADER_SIZE)
		|| (apiMsgType==WesSetupRequestEnum && len!=NCAPI_WESSETUPREQUEST_LENGTH)
		|| (apiMsgType==WesStatusEnum && len!=NCAPI_WESSTATUS_LENGTH)
		|| (apiMsgType==NodeInfoReplyEnum && len!=NCAPI_NODEINFOREPLY_LENGTH)
		|| (apiMsgType==RouteInfoRequestReplyEnum && len!=NCAPI_ROUTEINFOREQUESTREPLY_LENGTH)
		|| ((apiMsgType==NeighborListReplyEnum && len!=NCAPI_NEIGHBORLISTREPLY_LENGTH)&&(apiMsgType==NeighborListReplyEnum && len!=NCAPI_NEIGHBORLISTREPLY_EX_LENGTH))
		)
		return 0;
	if ((len+NCAPI_HOST_PREFIX_SIZE) > bufLength)
		return 0;
	*outLength = len + NCAPI_HOST_PREFIX_SIZE;
	return 1;
}

int NcApiIsValidApiFrame(uint8_t * buffer, uint16_t bufLength, uint16_t * outStartAt, uint16_t * outLength)
{
	*outStartAt = 0;
	return NcApiIsValidFrameProtocol(buffer, bufLength, outLength);
}

void NcApiGetMsgAsHostAck(uint8_t * buffer, tNcApiHostAckNack * p)
{
	// Message type "0x50: Acknowledge for previously sent packet":
	// Message type "0x51: Non-Acknowledge for previously sent packet":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	p->originId = (hiId << 8) | loId;
}

void NcApiGetMsgAsHostUappStatus(uint8_t * buffer, tNcApiHostUappStatus * p)
{
	// Message type "0x50: Acknowledge for previously sent packet":
	// Message type "0x51: Non-Acknowledge for previously sent packet":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	uint16_t hiSeq = buffer[4];
	uint16_t loSeq = buffer[5];
	
	p->originId = (hiId << 8) | loId;
	p->appSeqNo = (hiSeq << 8) | loSeq;

}

void NcApiGetMsgAsHostData(uint8_t * buffer, tNcApiHostData * p)
{
	// Message type "0x52: Host Data":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t payloadLen = buffer[1] - NCAPI_HOSTDATA_HEADER_SIZE;
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	uint16_t hiAge = buffer[4];
	uint16_t loAge = buffer[5];
	p->originId = (hiId << 8) | loId;
	p->packageAge = (hiAge << 8) | loAge;
	p->port = buffer[6];
	p->payloadLength = payloadLen;
	p->payload = buffer + NCAPI_HOSTDATA_MIN_LENGTH;
}

void NcApiGetMsgAsHostDataHapa(uint8_t * buffer, tNcApiHostDataHapa * p)
{
	// Message type "0x53: Host Data HAPA":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t payloadLen = buffer[1] - NCAPI_HOSTDATAHAPA_HEADER_SIZE;
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	uint32_t age1 = buffer[4];
	uint32_t age2 = buffer[5];
	uint32_t age3 = buffer[6];
	uint32_t age4 = buffer[7];
	p->originId = (hiId << 8) | loId;
	p->packageAge = (age1 << 24) | (age2 << 16) | (age3 << 8) | age4;
	p->port = buffer[8];
	p->payloadLength = payloadLen;
	p->payload = buffer + NCAPI_HOSTDATAHAPA_MIN_LENGTH;
}

void NcApiGetMsgAsHostUappData(uint8_t * buffer, tNcApiHostUappData * p)
{
	// Message type "0x54: Host Data Unacknowledged":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t payloadLen = buffer[1] - NCAPI_HOSTUAPPDATA_HEADER_SIZE;
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	uint16_t hiAge = buffer[4];
	uint16_t loAge = buffer[5];
	p->originId = (hiId << 8) | loId;
	p->packageAge = (hiAge << 8) | loAge;
	p->port = buffer[6];
	p->appSeqNo = ((buffer[7] << 8) | buffer[8]) & 0x0fff; // Only 12 valid bits totally
	p->payloadLength = payloadLen;
	p->payload = buffer + NCAPI_HOSTUAPPDATA_MIN_LENGTH;
}

void NcApiGetMsgAsHostUappDataHapa(uint8_t * buffer, tNcApiHostUappDataHapa * p)
{
	// Message type "0x55: Host Data HAPA Unacknowledged":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t payloadLen = buffer[1] - NCAPI_HOSTUAPPDATAHAPA_HEADER_SIZE;
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	uint32_t age1 = buffer[4];
	uint32_t age2 = buffer[5];
	uint32_t age3 = buffer[6];
	uint32_t age4 = buffer[7];
	p->originId = (hiId << 8) | loId;
	p->packageAge = (age1 << 24) | (age2 << 16) | (age3 << 8) | age4;
	p->port = buffer[8];
	p->appSeqNo = ((buffer[9] << 8) | buffer[10]) & 0x0fff; // Only 12 valid bits totally
	p->payloadLength = payloadLen;
	p->payload = buffer + NCAPI_HOSTUAPPDATAHAPA_MIN_LENGTH;
}

void NcApiGetMsgAsNodeInfoReply(uint8_t * buffer, tNcApiNodeInfoReply * p)
{
	// Message type "0x58: Node Info Reply":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	p->nodeId = (buffer[2] << 8) | buffer[3];

	p->uid[0] = buffer[4];
	p->uid[1] = buffer[5];
	p->uid[2] = buffer[6];
	p->uid[3] = buffer[7];
	p->uid[4] = buffer[8];

	p->Type = (NcApiNodeType)buffer[9];
}

void NcApiGetMsgAsNeighborListReply(uint8_t * buffer, tNcApiNeighborListReply * p)
{
	// Message type "0x59: Neighbor List Reply":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t si,di;
	di=0;
	for(si=2; si<(36+2); si+=3)
	{
		if ((buffer[si]!=0xff)||(buffer[si+1]!=0xff))
		{
			p->Neighbor[di].nodeId = (buffer[si]<<8) | buffer[si+1];
			p->Neighbor[di].RSSI = buffer[si+2];
			di++;
		}
	}	
	p->NeighborsCount=di;
}

void NcApiGetMsgAsRouteInfoRequestReply(uint8_t * buffer, tNcApiRouteInfoRequestReply * p)
{
	// Message type "0x5c: Route Info Request Reply":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.

	uint8_t i;
	for(i=0; i<16; i++)
	{
		p->Bitmap[i] = buffer[i+2];
	}
}




void NcApiGetMsgAsNetCmdResponse(uint8_t * buffer, tNcApiNetCmdReply * p)
{
	// Message type "0x5a: Network Command Reply":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	uint8_t payloadLen = buffer[1] - NCAPI_NETCMDRESPONSE_HEADER_SIZE;
	uint16_t hiId = buffer[2];
	uint16_t loId = buffer[3];
	p->originId = (hiId << 8) | loId;
	p->cmd = (NcApiNetCmdValues)buffer[4];
	p->payloadLength = payloadLen;
	p->payload = buffer + NCAPI_NETCMDRESPONSE_MIN_LENGTH;

}

void NcApiGetMsgAsWesStatus(uint8_t * buffer, tNcApiWesStatus * p)
{
	// Message type "0x60: WES Status":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	p->Status = buffer[2];
}

void NcApiGetMsgAsWesSetupRequest(uint8_t * buffer, tNcApiWesSetupRequest * p)
{
	// Message type "0x61: WES Setup Request":
	// Unpack the received message taking into account that data 
	// is exchanged over the interface in Big Endian byte order.
	p->uid[0] = buffer[2];
	p->uid[1] = buffer[3];
	p->uid[2] = buffer[4];
	p->uid[3] = buffer[5];
	p->uid[4] = buffer[6];
	p->appFuncType = buffer[7];
}
