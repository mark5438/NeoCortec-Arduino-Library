/*******************************************************************************
 * @file SAPIParser.cpp
 * @date 2023-08-04
 * @author Markus Rytter (markus.r@live.dk)
 *
 * @copyright Copyright (c) 2023
 *
 ******************************************************************************/

/**
 * @addtogroup NeoMesh
 * @{
 */

/*******************************************************************************
 *    Private Includes
 ******************************************************************************/

#include "SAPIParser.h"

#include <stdio.h>
#include <stdlib.h>

#include <Arduino.h>
#include <HardwareSerial.h>

/*******************************************************************************
 *    Public Class/Functions
 ******************************************************************************/

void SAPIParser::push_char(uint8_t c)
{
    if(this->cursor == 0 && c != SAPI_COMMAND_HEADER)
        return; // Must start with message header
    this->buffer[this->cursor ++] = c;
    this->check_for_message();
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

/*******************************************************************************
 *    Private Class/Functions
 ******************************************************************************/

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
        // The last byte of the message did not match the tail
        for(int i = 0; i < this->cursor; i++)
        {
            // Check if another command header is present within the buffer
            if(this->buffer[i] == SAPI_COMMAND_HEADER)
            {
                // Another command header is present in the buffer at position i
                // Shift all elements i to the left, so the command header will be at index 0
                for(int j = i, k = 0; j < this->cursor; j++, k++)
                    this->buffer[k] = this->buffer[j];
                break;
            }
        }
        this->cursor = 0;   // Ignore message
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

/*******************************************************************************/

/** @} addtogroup end */
