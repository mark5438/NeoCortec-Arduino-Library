/*******************************************************************************
 * @file SAPIParser.h
 * @date 2023-08-04
 * @author Markus Rytter (markus.r@live.dk)
 *
 * @copyright Copyright (c) 2023
 *
 *******************************************************************************/

/**
 * @addtogroup NeoMesh
 * @{
 */

#ifndef SAPI_PARSER_H
#define SAPI_PARSER_H

/*******************************************************************************
 *    Includes
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 *    Defines
 ******************************************************************************/

#define SAPI_COMMAND_HEADER 0x3E
#define SAPI_COMMAND_TAIL 0x21
#define MINIMUM_MESSAGE_LENGTH 5

/*******************************************************************************
 *    Type defines
 ******************************************************************************/

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

/*******************************************************************************
 *    Class prototypes
 ******************************************************************************/
/**
* @brief Class to parse messages from system interface
*/
class SAPIParser
{
public:
    /**
    * @brief Push new character to buffer
    * @param c New character
    */
    void push_char(uint8_t c);

    /**
    * @brief See if a message is received but not yet read
    * @return True if a message is pending. False otherwise
    */
    bool message_available();

    /**
    * @brief Get latest message
    * message_available() should be called before this
    * If no new message is received, the same message will
    * be returned as last time this function was called
    */
    tNcSapiMessage get_pending_message();

private:
    uint8_t buffer[32];
    tNcSapiMessage pending_message;
    bool is_message_pending = false;
    int cursor = 0;

    void check_for_message();
    void parse_message();
};

/*******************************************************************************/
/** @} addtogroup end */

#endif  // SAPI_PARSER_H