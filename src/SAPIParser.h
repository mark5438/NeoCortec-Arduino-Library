#include <stdint.h>

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

typedef enum {
    ReadWrite = 3,
    ReadOnly  = 2
} tNcSAPISettingAccessRight;

typedef struct {
    uint8_t setting_id;
    uint8_t value_length;
    uint8_t access_right;
} tNcSapiSetting;

typedef struct {
    uint8_t * setting_value;
    uint8_t value_length;
} tNcSapiSettingValue;

typedef void (*LoginOKCallback) ();
typedef void (*LoginErrorCallback) ();
typedef void (*BootloaderStartedCallback) ();
typedef void (*ProtocolStartedCallback) ();
typedef void (*ProtocolErrorCallback) ();
typedef void (*SettingListOutputCallback) (tNcSapiSetting * settings, uint8_t settings_length);
typedef void (*SettingValueResponseCallback) (tNcSapiSettingValue * setting_value);

class SAPIParser
{
public:
    void push_char(uint8_t c);

    LoginOKCallback login_ok_callback = 0;
    LoginErrorCallback login_error_callback = 0;

private:
    uint8_t buffer[32];
    uint8_t cursor = 0;
    void check_for_message();
    void parse_message();
};