//
//  serial.h
//  JKFoundation
//
//  Created by Corey Lange on 1/28/15.
//  Copyright (c) 2015 Apple. All rights reserved.
//

#ifndef bdfu_serial_h
#define bdfu_serial_h

#include <stdint.h>
#include "byte_fifo.h"
#include <pthread.h>
#include <termios.h>

//Settings
#define SETTINGS_DEFAULT                    0x00 //8N1
#define SETTINGS_SEVEN_BITS                 0x10 //7N1
#define SETTINGS_EVEN_PARITY                0x01 //8E1
#define SETTINGS_SEVEN_BITS_EVEN_PARITY     0x11 //7E1
#define SETTINGS_ODD_PARITY                 0x03 //8O1
#define SETTINGS_SEVEN_BITS_ODD_PARITY      0x13 //7O1
#define SETTINGS_TWO_STOP_BITS              0x04

typedef struct {
    byte_fifo_t rx_buffer;
    pthread_t thread;
    pthread_mutex_t lock;
    int port;
    char portname[128];
} serial_port_t;

int open_serial_port(const char* portname, speed_t baudrate, uint32_t settings, serial_port_t* port);
int close_serial_port(serial_port_t* port);
int serial_write_byte(serial_port_t* port, uint8_t byte);
int serial_write_string(serial_port_t* port, uint8_t* string, uint32_t size);
int serial_get_byte(serial_port_t* port, uint8_t* byte, uint32_t timeout);
int serial_get_line(serial_port_t* port, uint8_t* buffer, uint32_t size, uint32_t timeout);

// add by MT
int serial_get_line_endMark(serial_port_t* port, uint8_t* buffer, uint32_t size, char* end_mark, uint32_t timeout);
uint16_t byte_fifo_copy_endMark(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size, char* end_mark);
int serial_write_Hex(serial_port_t* port, char* string, int size);
int ReadByteStr(serial_port_t* port,int timeOut,int len);
#endif
