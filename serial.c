//
//  serial.c
//  JKFoundation
//
//  Created by Corey Lange on 1/28/15.
//  Copyright (c) 2015 Apple. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <IOKit/serial/ioss.h>
#include "byte_fifo.h"
#include "serial.h"

//#define SERIAL_DEBUG

static uint32_t serial_timer;

static void sigAlarmHandler(int sig) {
	if(serial_timer) serial_timer--;
}

static void startMsTimer(void) {
	struct itimerval tval = { { 0, 1000}, {0, 1000}};
    signal(SIGALRM, sigAlarmHandler);
    
	// Start the ms interval timer
	setitimer(ITIMER_REAL, &tval, NULL);
}

static void stopMsTimer(void) {
	struct itimerval tval = { {0,0}, {0,0}};
    
	// Stop the ms interval timer
	setitimer(ITIMER_REAL, &tval, NULL);
    
    signal(SIGALRM, SIG_IGN);
}

static void *byteHandler(void *refcon) {
	ssize_t rval;
	uint8_t c;
    serial_port_t* port = (serial_port_t*)refcon;
	
	// Change the port to blocking.
	//fcntl(port->port, F_SETFL, 0);
	
	while(1) {
		rval = read(port->port, &c, 1);
		pthread_testcancel();

		if(rval == 1) {
#ifdef SERIAL_DEBUG
            printf("GOT BYTE! %02X\n",c);
#endif
            pthread_mutex_lock(&port->lock);
			byte_fifo_push(&port->rx_buffer, c);
            pthread_mutex_unlock(&port->lock);
		} else if( (rval == -1) && (errno != EAGAIN) ) {
			// Comm error so exit the app
			perror("FATAL: Communications Error");
			raise(SIGINT);
			pthread_exit(NULL);
		}
        usleep(5000);
	}
}

int open_serial_port(const char* portname, speed_t baudrate, uint32_t settings, serial_port_t* port)
{
    speed_t speed = 1250000;
    struct termios options;

    if(portname) {
        port->port = open(portname,  O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    } else {
        fprintf(stderr, "No Serial port specified.\r\n");
        return -1;
    }
    
    if(port->port == -1) {
        fprintf(stderr, "Unable to open port: %s\r\n", portname);
        return -1;
    }
    
    // Get the current options for the port...
    tcgetattr(port->port, &options);
    
    //Set the baud rates
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    
    // Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);
    
    // Default - 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~PARODD;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= (settings & 0x10) ? CS7 : CS8;
    
    // Make setting changes
    if( settings & 0x01 ) {
        if( settings & 1 ) {
            //Enable Parity
            options.c_cflag |= PARENB;
            if( settings & 2 ) {
                //Enable Odd Parity
                options.c_cflag |= PARODD;
            }
        }
        if( settings & 4 ) {
            //Two Stop Bits
            options.c_cflag |= CSTOPB;
        }
    }
    
    // Disable cannonical processing
    options.c_lflag &= ~ICANON;
    
    // Set the timeout and minimum chars
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;
    
    // Set the new options for the port...
    tcsetattr(port->port, TCSANOW, &options);
    
    // set nonstandard speed
    if (baudrate > B115200) {
        int ret;
        ret = ioctl(port->port, IOSSIOSPEED, &baudrate);
        
        if (ret) {
            fprintf(stderr, "failed to set port speed to %u\r\n", (unsigned int)speed);
            return -1;
        }
    }
    
    byte_fifo_init(&port->rx_buffer);
    
    pthread_mutex_init(&port->lock, NULL);
    
    pthread_attr_t	attr;
    pthread_attr_init ( &attr );
    pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_DETACHED );
    pthread_create(&port->thread,  &attr, byteHandler, port);
    
    return 0;
}

int close_serial_port(serial_port_t* port)
{
    pthread_cancel(port->thread);
    pthread_join(port->thread, NULL);
    if(port->port >= 0) {
        close(port->port);
    }
    port->port = -1;
    return 0;
}

int serial_write_byte(serial_port_t* port, uint8_t byte)
{
#ifdef SERIAL_DEBUG
    printf("SEND BYTE %02X\n",byte);
#endif
    if( write(port->port, &byte, 1) < 0 ) {
        printf("Error sending byte: %s(%d).\r\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}

int serial_write_string(serial_port_t* port, uint8_t* string, uint32_t size)
{
    int i;
    for (i = 0; i < size; i++) {
        if( serial_write_byte(port, string[i]) < 0 )
            return -1;
    }
    return 0;
}

int serial_get_byte(serial_port_t* port, uint8_t* byte, uint32_t timeout)
{
    //Set timeout
    serial_timer = timeout;
    
    startMsTimer();
    while (serial_timer && !byte_fifo_count(&port->rx_buffer)) {
        usleep(10000);
    }
    stopMsTimer();
    
    if (byte_fifo_count(&port->rx_buffer)) {
        pthread_mutex_lock(&port->lock);
        *byte = byte_fifo_pop(&port->rx_buffer);
        pthread_mutex_unlock(&port->lock);
    } else {
        return -1;
    }
    
    return 0;
}

int serial_get_line(serial_port_t* port, uint8_t* buffer, uint32_t size, uint32_t timeout)
{
    //Set timeout
    serial_timer = timeout;
    
    startMsTimer();
    while (serial_timer && !byte_fifo_match(&port->rx_buffer, '\r')) {
        usleep(10000);
    }
    stopMsTimer();
    
    if (byte_fifo_match(&port->rx_buffer, '\r')) {
        pthread_mutex_lock(&port->lock);
//        byte_fifo_copy_line(&port->rx_buffer, buffer, (uint16_t)size);
        byte_fifo_get_line(&port->rx_buffer, buffer, (uint16_t)size);
        pthread_mutex_unlock(&port->lock);
    } else {
        return -1;
    }
    
    return 0;
}

// ======= add by MT =======
uint16_t byte_fifo_copy_endMark(byte_fifo_t* fifo, uint8_t* buffer, uint16_t buffer_size, char* end_mark)
{
    uint16_t i, j = 0;
    
    for(i = fifo->read_index; i != fifo->write_index; i++) {
        i = i & BYTE_FIFO_SIZE_MASK;
        
        //Sanity check: If we run out of bytes in the buffer, just return
        if( j >= (buffer_size - 1) ) {
            buffer[j] = 0;
            return j;
        }
        char *buf = (char*)buffer;
        char *wTmp;
        wTmp = strstr(buf,end_mark);
        if (NULL != wTmp) {
            buffer[j] = 0;
            return j;
        }
        buffer[j++] = fifo->data[i];
        fifo->read_index += i;
    }
    
    return 0;
}

int serial_get_line_endMark(serial_port_t* port, uint8_t* buffer, uint32_t size, char* end_mark, uint32_t timeout)
{
    //Set timeout
    serial_timer = timeout;
    
    startMsTimer();
    while (serial_timer && !byte_fifo_match(&port->rx_buffer, '\r')) {
        usleep(10000);
    }
    stopMsTimer();
    
    if (byte_fifo_match(&port->rx_buffer, '\r')) {
        pthread_mutex_lock(&port->lock);
        byte_fifo_copy_endMark(&port->rx_buffer, buffer, (uint16_t)size, end_mark);
        pthread_mutex_unlock(&port->lock);
    } else {
        return -1;
    }
    
    return 0;
}
#define SERIALPORT_MAXBUFSIZE  8192UL
int serial_write_Hex(serial_port_t* port, char* string, int size)
{
    if( write(port->port, string, size) < 0 ) {
        printf("Error sending byte: %s(%d).\r\n", strerror(errno), errno);
        return -1;
    }
    return 0;
}

int ReadByteStr(serial_port_t* port,int timeOut,int len)
{
    char chr_to_str[1024] = "";
    size_t num = 0;
    num = read(port->port, &chr_to_str, 1024);
    
    return 0;
}




