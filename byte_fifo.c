#include <stdio.h>
#include <stdint.h>
#include "byte_fifo.h"

//#define FIFO_DEBUG printf
#define FIFO_DEBUG(...)

void byte_fifo_init(byte_fifo_t* fifo)
{
    fifo->read_index = 0;
    fifo->write_index = 0;
    fifo->count = 0;
}

uint16_t byte_fifo_count(byte_fifo_t* fifo)
{
    return fifo->count;
}

uint16_t byte_fifo_push(byte_fifo_t* fifo, uint8_t byte)
{
    if(fifo->count < MAX_BYTE_FIFO_SIZE) {
        FIFO_DEBUG("Pushing Byte to idx %d ", fifo->write_index);
        fifo->data[fifo->write_index] = byte;
        fifo->write_index = (fifo->write_index + 1) & BYTE_FIFO_SIZE_MASK;
        fifo->count = fifo->count + 1;
    }
    FIFO_DEBUG("Count is now %d\n",fifo->count);
    return fifo->count;
}

uint16_t byte_fifo_push_many(byte_fifo_t* fifo, uint8_t* bytes, uint16_t number_of_bytes)
{
    uint16_t i;

    for(i = 0; i < number_of_bytes; i++)
        if(byte_fifo_push(fifo,bytes[i]) >= MAX_BYTE_FIFO_SIZE)
            break;
    return fifo->count;
}

uint8_t byte_fifo_peek(byte_fifo_t* fifo)
{
    if(fifo->count) {
        return fifo->data[fifo->read_index];
    }
    return 0;
}

uint8_t byte_fifo_peek_tail(byte_fifo_t* fifo)
{
    if(fifo->count) {
        return fifo->data[(fifo->write_index - 1) & BYTE_FIFO_SIZE_MASK];
    }
    return 0;
}


uint8_t byte_fifo_pop(byte_fifo_t* fifo)
{
    uint8_t byte = 0;
    if(fifo->count) {
        FIFO_DEBUG("Popping Byte from idx %d ", fifo->read_index);
        byte = fifo->data[fifo->read_index];
        //fifo->data[fifo->read_index] = 0;
        fifo->read_index = (fifo->read_index + 1) & BYTE_FIFO_SIZE_MASK;
        fifo->count = fifo->count - 1;
    }
    FIFO_DEBUG("Count is now %d\n",fifo->count);
    return byte;
}

uint16_t byte_fifo_pop_many(byte_fifo_t* fifo, uint8_t* bytes, uint16_t number_of_bytes)
{
    uint16_t i;

    for(i = 0; i < number_of_bytes; i++) {
        if(fifo->count == 0) {
            break;
        }
        bytes[i] = byte_fifo_pop(fifo);
    }
    return i;
}

uint8_t byte_fifo_pop_tail(byte_fifo_t* fifo)
{
    uint8_t byte = 0;
    if(fifo->count) {
        byte = fifo->data[(fifo->write_index - 1) & BYTE_FIFO_SIZE_MASK];
        fifo->write_index = (fifo->write_index - 1) & BYTE_FIFO_SIZE_MASK;
        fifo->count = fifo->count - 1;
    }
    return byte;
}

uint16_t byte_fifo_get_next_token(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size, const uint8_t* sep)
{
    uint8_t data;
    uint8_t* current_separator;
    uint16_t index = 0;
    uint8_t skip_byte = 0;
    
    //Iterate through the fifo
    while( fifo->count > 0 ) {
        data = byte_fifo_pop(fifo);
        //Check all of the separators
        for(current_separator = (uint8_t*)sep; *current_separator != '\0'; current_separator++) {
            if( data == *current_separator ) {
                //If we find a match to a separator, we're done (maybe)
                if( index > 0 ) {
                    buffer[index] = 0;
                    return index;
                } else {
                    //If we haven't loaded anything into the buffer, keep trying until the fifo is empty
                    skip_byte = 1;
                    break;
                }
            }
        }
        
        if( skip_byte ) {
            skip_byte = 0;
            continue;
        }
        
        //Copy the data over
        buffer[index++] = data;
        //Make sure we don't overflow the buffer
        if( index >= (buffer_size - 1) )
            break;
    }
    buffer[index] = 0;
    return index;
}

uint16_t byte_fifo_get_line(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size)
{
    uint16_t j;
    uint8_t line_finished = 0;

    for(j = 0; j < buffer_size; j++) {
        //Sanity check: If we run out of bytes in the fifo, just return
        if(fifo->count == 0) {
            buffer[j] = 0;
            return j;
        }

        if(line_finished) {
            //Copy all of the newline and carriage return bytes
            if((byte_fifo_peek(fifo) != '\n') && (byte_fifo_peek(fifo) != '\r')) {
                buffer[j] = 0;
                return j;
            } else {
                buffer[j] = byte_fifo_pop(fifo);
            }
        } else {
            if((byte_fifo_peek(fifo) == '\n') || (byte_fifo_peek(fifo) == '\r')) {
                line_finished = 1;
            }
            buffer[j] = byte_fifo_pop(fifo);
        }
    }

    return 0;
}

uint16_t byte_fifo_copy_line(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size)
{
    uint16_t i, j = 0;
    
    for(i = fifo->read_index; i != fifo->write_index; i++) {
        i = i & BYTE_FIFO_SIZE_MASK;
        
        //Sanity check: If we run out of bytes in the buffer, just return
        if( j >= (buffer_size - 1) ) {
            buffer[j] = 0;
            return j;
        }
        
        if( (fifo->data[i] == '\n') || (fifo->data[i] == '\r') ) {
            buffer[j] = 0;
            return j;
        }
        buffer[j++] = fifo->data[i];
    }
    
    return 0;
}

uint8_t byte_fifo_match(byte_fifo_t* fifo, uint8_t byte)
{
    uint16_t i;
    
    for(i = fifo->read_index; i != fifo->write_index; i++) {
        i = i & BYTE_FIFO_SIZE_MASK;
        if(fifo->data[i] == byte) {
            return 1;
        }
    }
    
    return 0;
}

uint8_t byte_fifo_line_ready(byte_fifo_t* fifo)
{
    return byte_fifo_match(fifo, '\n') || byte_fifo_match(fifo, '\r');
}
