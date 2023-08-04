#include "SAPIParser.h"

#include <stdio.h>
#include <stdlib.h>

#include <Arduino.h>
#include <HardwareSerial.h>

void SAPIParser::push_char(uint8_t c)
{
    if(this->cursor == 0 && c != SAPI_COMMAND_HEADER)
        return; // Must start with message header
    this->buffer[this->cursor ++] = c;
    this->check_for_message();
}

void SAPIParser::check_for_message()
{
    if(this->cursor + 1 < MINIMUM_MESSAGE_LENGTH)
        return;
    
    // Cursor more than 5:
    int length = this->buffer[1];
    if(this->cursor < length + 2)
        return;

    if(this->buffer[length + 1] == SAPI_COMMAND_TAIL)
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
    this->pending_message.data_length = message_length - 3;
    this->pending_message.command = this->buffer[3];
    for(int i = 0; i < this->pending_message.data_length; i++)
    {
        this->pending_message.data[i] = this->buffer[4 + i];
    }
    this->is_message_pending = true;
    cursor = 0;
}

bool SAPIParser::message_available()
{
    return this->is_message_pending;
}

tNcSapiMessage SAPIParser::get_pending_message()
{
    this->is_message_pending = false;
    return this->pending_message;
}