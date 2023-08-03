#ifdef gekgiutrehgiurthuirthrth

#include "NcApi.h"


// This example configures one instance of NcApi using one common call back function.
// It shows a bare minimum implementation of what is required in order to receive and send messages.
// As this example actually is part of the unit-test tool for NcApi, some printing and logging of actions and messages is included in the callbacks as well.
// These should, of course, be left out in a real-world implementation.
// It is recommened to walk through ExampleSetupNcApi() and ExampleSendAcknowledged() defined at the end of this file.


// Global declarations expected by NcApi
tNcApi g_ncApi[1];
uint8_t g_numberOfNcApis = 1;
tNcApiRxHandlers ncRx;

// Global functions expected by NcApi
NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t * finalMsg, uint8_t finalMsgLength)
{
	// Write to particular UART indexed by n
	return NCAPI_OK;
}

void NcApiSupportMessageWritten(uint8_t n, void * callbackToken, uint8_t * finalMsg, uint8_t finalMsgLength)
{
	
}

void NcApiSupportMessageReceived(uint8_t n,void * callbackToken, uint8_t * msg, uint8_t msgLength)
{
	NcApiExecuteCallbacks(n,msg,msgLength);
}
 

// Handle CTS and RX interrupts
void ExamplePassthroughCts()
{
	NcApiCtsActive(0);
}

void ExamplePassthroughRx(uint8_t byte)
{
	NcApiRxData(0, byte);
	
}

void ExamplePassthroughNwu()
{
	NcApiCallbackNwuActive(0);
}


// Sample implementation of generic message received handler merely providing the bytes.
// Always called before any of the strongly typed message handlers are called.
void ExampleReadCallback(uint8_t n, uint8_t * msg, uint8_t msgLength)
{
	uint16_t i;
	printf("Raw UART data received: ");
	for (i=0; i<msgLength; i++)
		printf( "%02x ", msg[i] );
	printf("\r\n");
}

void ExampleHostAckCallback(uint8_t n, tNcApiHostAckNack * p)
{
	printf( "My previous package to node node %04x was successfully delivered\r\n", p->originId );
}

void ExampleHostNAckCallback(uint8_t n, tNcApiHostAckNack * p)
{
	printf( "My previous package to node node %04x was not delivered\r\n", p->originId );
}

void ExampleHostDataCallback(uint8_t n, tNcApiHostData * p)
{
	printf
	(
		"I have received %d bytes from node %04x. They are %dms old\r\n",
		p->payloadLength,
		p->originId,
		p->packageAge * 125
	);
}

void ExampleHostDataHapaCallback(uint8_t n, tNcApiHostDataHapa * p)
{
	printf
	(
		"I have received %d bytes from node %x4. They are %dms old\r\n",
		p->payloadLength,
		p->originId,
		((double)p->packageAge) / 524.288
	);
}

void ExampleWesSetupRequestCallback(uint8_t n, tNcApiWesSetupRequest * p)
{
	n;
}

void ExampleWesStatusCallback(uint8_t n, tNcApiWesStatus * p)
{
	n;
}

// Sample implementation of how to send one Unacknowledged command message
void ExampleSendUnacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t * payload, uint8_t payloadLen)
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

// Sample implementation of how to send one Acknowledged command message
void ExampleSendAcknowledged(uint16_t destNodeId, uint8_t port, uint8_t * payload, uint8_t payloadLen)
{
	tNcApiSendAckParams args;
	NcApiErrorCodes apiStatus;
	args.msg.destNodeId = destNodeId;
	args.msg.destPort = port;
	args.msg.payload = payload;
	args.msg.payloadLength = payloadLen;
	args.callbackToken = &g_ncApi;
	apiStatus = NcApiSendAcknowledged(0, &args);
	if (apiStatus != NCAPI_OK)
	{
		; // Application specific
	}
}

// Sample implementation of how to send one WesCmd
void ExampleSendWesCmd(NcApiWesCmdValues cmd)
{
	tNcApiWesCmdParams args;
	args.msg.cmd = cmd;
	args.callbackToken = &g_ncApi;
	NcApiSendWesCmd( 0, &args );
}

// Sample implementation of how to send one WesResponse
void ExampleSendWesResponse(uint64_t uid, uint16_t nodeId)
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

// Sample implementation of how to setup global rx-handlers and initialize NcApi.
void ExampleSetupNcApi()
{
	tNcApiRxHandlers * rxHandlers = &ncRx;
	memset( rxHandlers, 0, sizeof(tNcApiRxHandlers));

	rxHandlers->pfnReadCallback = ExampleReadCallback;
	rxHandlers->pfnHostAckCallback = ExampleHostAckCallback;
	rxHandlers->pfnHostNAckCallback = ExampleHostNAckCallback;
	rxHandlers->pfnHostDataCallback = ExampleHostDataCallback;
	rxHandlers->pfnHostDataHapaCallback = ExampleHostDataHapaCallback;
	rxHandlers->pfnWesSetupRequestCallback = ExampleWesSetupRequestCallback;
	rxHandlers->pfnWesStatusCallback = ExampleWesStatusCallback;
	
	NcApiInit();
	
	g_ncApi[0].NcApiRxHandlers=&ncRx;
}

#endif
