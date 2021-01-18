/* 
 * File:   byte_fifo.h
 * JKFoundation
 *
 * Created on January 26, 2015, 2:36 PM
 */

#ifndef BYTE_FIFO_H
#define	BYTE_FIFO_H

#include <stdint.h>

#define MAX_BYTE_FIFO_SIZE      1024
#define BYTE_FIFO_SIZE_MASK     0x3FF

typedef struct {
    uint8_t data[MAX_BYTE_FIFO_SIZE];
    uint16_t write_index;
    uint16_t read_index;
    uint16_t count;
} byte_fifo_t;

void byte_fifo_init(byte_fifo_t* fifo);
uint16_t byte_fifo_count(byte_fifo_t* fifo);
uint16_t byte_fifo_push(byte_fifo_t* fifo, uint8_t byte);
uint16_t byte_fifo_push_many(byte_fifo_t* fifo, uint8_t* bytes, uint16_t number_of_bytes);
uint8_t byte_fifo_peek(byte_fifo_t* fifo);
uint8_t byte_fifo_peek_tail(byte_fifo_t* fifo);
uint8_t byte_fifo_pop(byte_fifo_t* fifo);
uint16_t byte_fifo_pop_many(byte_fifo_t* fifo, uint8_t* bytes, uint16_t number_of_bytes);
uint8_t byte_fifo_pop_tail(byte_fifo_t* fifo);
uint16_t byte_fifo_get_next_token(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size, const uint8_t* sep);
uint16_t byte_fifo_get_line(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size);
uint16_t byte_fifo_copy_line(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size);
uint8_t byte_fifo_match(byte_fifo_t* fifo, uint8_t byte);
uint8_t byte_fifo_line_ready(byte_fifo_t* fifo);


#endif	/* BYTE_FIFO_H */

