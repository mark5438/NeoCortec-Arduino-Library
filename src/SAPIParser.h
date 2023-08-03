#include <stdint.h>

#ifndef SAPI_PARSER_H
#define SAPI_PARSER_H

#define SAPI_COMMAND_HEADER 0x3E
#define SAPI_COMMAND_TAIL 0x21
#define MINIMUM_MESSAGE_LENGTH 5

typedef enum {
    LoginOK            = 0x80,
    LoginError         = 0x81,
    BootloaderStarted  = 0x82,
    ProtocolStarted    = 0x83,
    ProtocolError      = 0x84,
    ProtocolListOutput = 0x85,
    SettingValue       = 0x86
} tNcApiSapiMessageType;

typedef struct {
    uint8_t command;
    uint8_t data[32];
    uint8_t data_length;
} tNcSapiMessage;

class SAPIParser
{
public:
    SAPIParser(uint8_t n);
    void push_char(uint8_t c);
    bool message_available();
    tNcSapiMessage get_pending_message();

    // Should be private
    int cursor = 0;

private:
    uint8_t n;
    uint8_t buffer[32];
    tNcSapiMessage pending_message;
    bool is_message_pending = false;

    void check_for_message();
    void parse_message();
};

#endif  // SAPI_PARSER_H