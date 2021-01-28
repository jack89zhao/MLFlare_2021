//
//  JKCL200A.c
//  CL200A
//
//  Created by Jackie Wang on 2019/10/19.
//  Copyright © 2019 mt-007. All rights reserved.
//

#include "JKCL200A.h"
#include "cserial.h"
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

typedef enum {
    MLLogInfo = 0,
    MLLogWarning,
    MLLogError,
} MLLogLevel;   // log等级级别

extern void Logger(MLLogLevel type, char* message, ...);

typedef int serialport;

static serialport port;
static char gBuffer[8 * 1024];
bool isConnected;
int gTimeout;
char gReceptor[3];

void JKCombineCommand(const char* cmd1, const char * cmd2, char command[]);
char* JkReadLine(void);
char* JKReadToMask(const char *mask);
void JKWriteCommandString(char cmd[], float *result1, float *result2, float *result3);
int JKGetBccCode(const char *cmd, int size, char bccCode[]);
float JKExactValue(char buffer[], int location);

/* ========================================================================= */

void JKSetCL200ATimeout(int timeout) {
    if (timeout > 0) {
        gTimeout = timeout;
    }
}

int JKConnectCL200A(const char* portname, const char *receptor) {
    port = serial_open_file(portname, 9600);
    
    if (port != -1) {
        gTimeout = 2000;
        isConnected = true;
        memset(gBuffer, 0, 8 * 1024);
        serial_set_timeout(port, 1000);
        serial_set_attr(port, 7, 'E', 1, 0);
        JKSetReceptor(receptor);
        Logger(MLLogInfo, "<JKCL200A>: Connect luxmeter success.\n");
    } else {
        isConnected = false;
        Logger(MLLogInfo, "<JKCL200A>: Connect luxmeter fail.\n");
    }
    
    return isConnected;
}

/* Close the CL200A serial port */
int JKDisconnectCL200A(void) {
    int rtn = 0;
    
    if (isConnected) {
        rtn = serial_close(port);
        isConnected = false;
        Logger(MLLogInfo, "<JKCL200A>: Disconnect luxmeter.\n");
    }
    
    return rtn;
}

/* set receptor value */
void JKSetReceptor(const char *receptor) {
    if (isConnected) {
        const int size = 15;
        char command[size];
        memset(command, 0, size);
        
        JKCombineCommand(receptor, "541   ", command);
        serial_send(port, command, size);
        usleep(200 * 1000);
        char *line = JkReadLine();
        usleep(500 * 1000);
        JKClearBuffer();
        Logger(MLLogInfo, "<JKCL200A>: Luxmeter command rtn: %s\n", line);
        
        JKCombineCommand("", "99551  0", command);
        serial_send(port, command, strlen(command));
        usleep(500 * 1000);

        JKCombineCommand(receptor, "4010  ", command);
        serial_send(port, command, strlen(command));
        usleep(200 * 1000);
        char *line1 = JkReadLine();
        usleep(500 * 1000);
        JKClearBuffer();
        Logger(MLLogInfo, "<JKCL200A>: Luxmeter command rtn: %s\n", line1);
        
        memset(gReceptor, 0, 3);
        strcpy(gReceptor, receptor);
        
        if (line != NULL) { free(line); }
        if (line1 != NULL) { free(line1); }
    }
}

/* Measurement data can be read X, Y, Z values */
void JKGetCL200AXYZ (float* x,  float* y, float* z) {
    JKWriteCommandString("011200", x, y, z);
}

/* Measurement data can be read EV, x', y' values */
void JKGetCL200AEvXY(float *Ev, float *x, float *y) {
    JKWriteCommandString("021200", Ev, x, y);
}

/* Measurement data can be read EV, u', v' values */
void JKGetCL200AEvUV(float* Ev, float* u, float* v) {
    JKWriteCommandString("031200", Ev, u, v);
}

/* Measurement data can be read X, Y, Z values; EV, x', y' values; EV, u', v' values */
void JKGetCL200AALL(float* XYZArray, float* EvXYArray, float* EvUVArray) {
    if (isConnected) {
        const int size = 15;
        char command[size];
        memset(command, 0, size);
        
        int cnt = 0;
        do {
            JKClearBuffer();
            JKCombineCommand("", "994021  ", command);
            serial_send(port, command, size);
            usleep(500 * 1000);
        } while (cnt++ < 3);
        
        JKClearBuffer();
        JKCombineCommand("", "994021  ", command);
        serial_send(port, command, size);
        sleep(1);
        
        JKCombineCommand(gReceptor, "011200", command);
        serial_send(port, command, size);
        usleep(175 * 1000);
        char *line = JkReadLine();
        printf("luxmeter: %s\n", line);
        
        XYZArray[0] = JKExactValue(line, 9);
        XYZArray[1] = JKExactValue(line, 15);
        XYZArray[2] = JKExactValue(line, 21);
        
        JKCombineCommand(gReceptor, "021200", command);
        serial_send(port, command, size);
        usleep(175 * 1000);
        char *line1 = JkReadLine();
        printf("luxmeter: %s\n", line1);
        
        EvXYArray[0] = JKExactValue(line1, 9);
        EvXYArray[1] = JKExactValue(line1, 15);
        EvXYArray[2] = JKExactValue(line1, 21);
        
        JKCombineCommand(gReceptor, "031200", command);
        serial_send(port, command, size);
        usleep(175 * 1000);
        char *line2 = JkReadLine();
        printf("luxmeter: %s\n", line2);
        
        EvUVArray[0] = JKExactValue(line2, 9);
        EvUVArray[1] = JKExactValue(line2, 15);
        EvUVArray[2] = JKExactValue(line2, 21);
        
        if (line != NULL) { free(line); }
        if (line1 != NULL) { free(line1); }
        if (line2 != NULL) { free(line2); }
    }
}

/* Clear the cache */
void JKClearBuffer(void) {
    if (isConnected) {
        char buffer[1024];
        memset(buffer, 0, 1024);

        if (serial_data_available(port, 100)) {
            serial_receive(port, buffer, 1024);
        }
    }
}

void JKWriteCommandString(char cmd[], float *result1, float *result2, float *result3) {
    if (isConnected && 0 != strcmp(cmd, "")) {
        const int size = 15;
        char command[size];
        memset(command, 0, size);
        
        int cnt = 0;
        do {
            JKClearBuffer();
            JKCombineCommand("", "994021  ", command);
            serial_send(port, command, size);
            usleep(500 * 1000);
        } while (cnt++ < 3);
        JKClearBuffer();
        JKCombineCommand("", "994021  ", command);
        serial_send(port, command, size);
        usleep(1000 * 1000);
        
        JKCombineCommand(gReceptor, cmd, command);
        serial_send(port, command, size);
        usleep(175 * 1000);
        char *line = JkReadLine();
        Logger(MLLogInfo, "<JKCL200A>: luxmeter: %s\n", line);
        
        *result1 = JKExactValue(line, 9);
        *result2 = JKExactValue(line, 15);
        *result3 = JKExactValue(line, 21);
        
        if (line != NULL) {
            free(line);
        }
    } else {
        Logger(MLLogInfo, "<JKCL200A>: luxmeter disconnected.\n");
    }
}

char* JKReadToMask(const char *mask) {
    char *buffer = (char *)malloc(1024 * sizeof(char));
    
    if (isConnected) {
        if (mask != NULL) {
            int time = 0;
            char tmpBuffer[256];
            memset(tmpBuffer, 0, 256);
            
            do {
                if (1 == serial_data_available(port, 200)) {
                    ssize_t size = serial_receive(port, tmpBuffer, 256);
                    
                    if (size > 0) {
                        strcat(gBuffer, tmpBuffer);
                    }
                    
                    // 找到 endmark之前的字符串
                    char *subStr = strtok(gBuffer, mask);
                    
                    if (subStr != NULL) {
                        strcpy(buffer, subStr);
                        
//                        if (0 != strcmp(gBuffer, "")) {
//                            char *otherStr = strtok(gBuffer, NULL);
                            memset(gBuffer, 0, 8 * 1024);
//                            strcpy(gBuffer, otherStr);
//                        }
                        
                        break;
                    }
                }
                time += 100;
                usleep(100 * 1000);
            } while (time < gTimeout);
        } else {
            if (serial_data_available(port, 1000)) {
                serial_receive(port, buffer, 1024);
            }
        }
    }
    
    return buffer;
}

char* JkReadLine() {
    return JKReadToMask("\n");
}

void JKCombineCommand(const char *cmd, const char *cmd1, char command[]) {
    ssize_t len1 = strlen(cmd);
    ssize_t len2 = strlen(cmd1);
    ssize_t length = len1 + len2;
    
    char combineCmd[length+1];
    memset(combineCmd, 0, length+1);
    
    for (int i = 0; i < length; i++) {
        if (i < len1) {
            combineCmd[i] = cmd[i];
        } else if (i < length) {
            combineCmd[i] = cmd1[i - len1];
        }
    }

    // get bcc code.
    char bccCode[3];
    JKGetBccCode(combineCmd, (int)length, bccCode);
    
    command[0] = 0x02;
    
    for (int j = 1; j < length + 4; j++) {
        if (j <= length) {
            command[j] = combineCmd[j-1];
        } else {
            continue;
        }
    }
    
    command[9] = 0x03;
    command[10] = bccCode[0];
    command[11] = bccCode[1];
    command[12] = 0x0D;         // '\r'
    command[13] = 0x0A;         // '\n'
}

int JKGetBccCode(const char *cmd, int size, char bccCode[]) {
    char code[2];
    memset(code, 0, 2);
    
    int bcc = cmd[0];
    
    for (int i = 1; i < size; i++) {
        bcc ^= cmd[i];
    }
    bcc ^= 0x03;
    
    code[0] = bcc / 16;
    code[1] = bcc % 16;
    
    for (int j = 0; j < 2; j++) {
        if (code[j] >= 0 && code[j] <= 9) {
            code[j] = '0' + code[j];
        } else if (code[j] >= 10 && code[j] < 15) {
            code[j] = 'A' + code[j];
        } else {
            code[j] = '\0';
        }
    }
    
    strcpy(bccCode, code);
    
    return bcc;
}

float JKExactValue(char buffer[], int location) {
    float number = 0;
    int length = (int)strlen(buffer);
    
    if (0 != strcmp(buffer, "") && length >= 30) {
        char substr[8];
        memset(substr, 0, 8);
        
        for (int i = 0; i < 6; i++) {
            substr[i] = buffer[location+i];
        }
        
        char character = substr[5];
        int exponent = (int)(character-'0') - 4;
        
        char symbol = substr[0];
        substr[0] = ' ';        // use space to replace symbol.
        substr[5] = '\0';       // use '\0' replace exponent value.
        
        number = atof(substr);  // convert string to float
        
        if (symbol == '-') {
            number = -number;   // minus
        }
        
        number = number * pow(10, exponent);
    }
    
    return number;
}
