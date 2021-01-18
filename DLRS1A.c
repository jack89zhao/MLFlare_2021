//
//  DLRS1A.c
//  serial
//
//  Created by Kerwin on 2019/7/16.
//  Copyright © 2019 Kerwin. All rights reserved.
//

#include "DLRS1A.h"
#include "serial.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef enum {
    MLLogInfo = 0,
    MLLogWarning,
    MLLogError,
} MLLogLevel;   // log等级级别

extern void Logger(MLLogLevel type, char* message, ...);

serial_port_t port;

bool ConnectDLRS1A(char *portname) {
    int openSer = open_serial_port(portname, 9600, SETTINGS_DEFAULT, &port);
    if (openSer == -1) {
        Logger(MLLogError, "<%s>: Fail to connect the loadcell, port name: %s\n", __func__, portname);
        return true;
    }else {
        Logger(MLLogInfo, "<%s>: Success to connect the laser device, port name: %s, fd: %d\n", __func__, portname, openSer);
        return false;
    }
}

void DisconnectDLRS1A(void) {
    close_serial_port(&port);
    Logger(MLLogInfo, "<%s>: Diconnect laser device\n", __func__);
//    printf("Closed Serial Port!\n");
}

double GetDLRS1AMeasureValue() {
    char* cmd = "SR,00,072\r\n";
    uint8_t* result = (uint8_t*)malloc(1024);
    serial_write_string(&port, (uint8_t*)cmd, (uint32_t)strlen(cmd));
//    printf("Send:%s", cmd);

    int n = 0, retry = 0;
    while(retry < 3 && n < 17) {
        sleep(1);
        serial_get_line(&port, result, 1024, 3000);
        n = (int)strlen( (char*)result );
        retry++;
    }
//    printf("Return:%s\n", result);
    Logger(MLLogInfo, "<%s>: Laser original data: %s, command: SR,00,072\\r\\n\n", __func__, result);
    
    char data[n];
    strncpy(data, (char*)result+10, n-10);
    
    double num = atof(data);
//    printf("%f\n", num);
    free(result);
    Logger(MLLogInfo, "<%s>: Laser measure value: %lf\n", __func__, num);
    
    return num;
}

double GetDLRS1AM0MeasureValue(void) {
    char* cmd = "M0\r\n";
    uint8_t* result = (uint8_t*)malloc(1024);
    serial_write_string(&port, (uint8_t*)cmd, (uint32_t)strlen(cmd));
//    printf("Send:%s", cmd);
    
    int n = 0, retry = 0;
    while(retry < 3 && n < 10) {
        sleep(1);
        serial_get_line(&port, result, 1024, 3000);
        n = (int)strlen( (char*)result );
        retry++;
    }
//    printf("Return:%s\n", result);
    Logger(MLLogInfo, "<%s>: Laser original data: %s, command: M0\\r\\n\n", __func__, result);
    
    char *data = (char *)malloc(n);
    if(n>3) {
        strncpy(data, (char*)result+3, n-3);
    }
    Logger(MLLogInfo, "<%s>: Laser M0 measure value: %lf\n", __func__, atof(data));
    
    return atof(data);
}

char* GetDLRS1AAllData() {
    char* cmd = "M0\r\n";
    uint8_t* result = (uint8_t*)malloc(1024);
    serial_write_string(&port, (uint8_t*)cmd, (uint32_t)strlen(cmd));
//    printf("Send:%s", cmd);

    int n = 0, retry = 0;
    while(retry < 3 && n < 10) {
        sleep(1);
        serial_get_line(&port, result, 1024, 3000);
        n = (int)strlen( (char*)result );
        retry++;
    }
//    printf("Return:%s\n", result);
    Logger(MLLogInfo, "<%s>: Laser original data: %s, command: M0\\r\\n\n", __func__, result);
    
    char *data = (char *)malloc(n);
    if(n>3) {
        strncpy(data, (char*)result+3, n-3);
    }
    Logger(MLLogInfo, "<%s>: Laser all measure values: %lf\n", __func__, data);
    
    return data;
}
