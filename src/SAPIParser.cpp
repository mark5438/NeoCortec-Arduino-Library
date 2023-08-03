#include "SAPIParser.h"

void SAPIParser::push_char(uint8_t c)
{
    if(this->cursor == 0 && c != SAPI_COMMAND_HEADER)
        return; // Must start with message header
    this->buffer[this->cursor ++] = c;
    this->check_for_message();
}

void SAPIParser::check_for_message()
{
    if(this->cursor < MINIMUM_MESSAGE_LENGTH)
        return;
    
    // Cursor more than 5:
    int length = this->buffer[1];
    if(this->cursor < length + 2)
        return;

    if(this->buffer[length + 2] == SAPI_COMMAND_TAIL)
    {
        this->parse_message();
    }
    else
    {
        this->cursor = 0;   // Ignore message
                            // TODO: Check if buffer contains SAPI_COMMAND_HEADER later on. Then shift
    }
}

void SAPIParser::parse_message()
{
    // When this function is called, it is assumed that there is a valid message frame
    uint8_t message_length = this->buffer[1];
    uint8_t data_length = message_length - 3;
    uint8_t cmd1 = this->buffer[2];
    uint8_t cmd2 = this->buffer[3];
    uint8_t data[data_length];

    for(int i = 0; i < data_length; i++)
    {
        data[i] = this->buffer[4 + i];
    }

    if(this->message_callback != 0)
        this->message_callback(cmd2, data, data_length);
}