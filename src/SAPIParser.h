#include <stdint.h>

#define SAPI_COMMAND_HEADER 0x3E
#define SAPI_COMMAND_TAIL 0x21
#define MINIMUM_MESSAGE_LENGTH 5

typedef void (*NcSapiMessageCallback) (uint8_t cmd, uint8_t * data, uint8_t data_length);

class SAPIParser
{
public:
    void push_char(uint8_t c);
    NcSapiMessageCallback message_callback = 0;

private:
    uint8_t buffer[32];
    uint8_t cursor = 0;
    void check_for_message();
    void parse_message();
};