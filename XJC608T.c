//
//  XJC608T.c
//  FS1536
//
//  Created by ViKing Lai on 2019/7/20.
//  Copyright © 2019 ViKing Lai. All rights reserved.
//
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <pthread.h>
#include "cserial.h"
#include "XJC608T.h"
#include <string.h>

typedef enum {
    MLLogInfo = 0,
    MLLogWarning,
    MLLogError,
} MLLogLevel;   // log等级级别

extern void Logger(MLLogLevel type, char* message, ...);

pthread_mutex_t lock;

int ConnectXJCPort(const char* portName) {
    int fd = serial_open_file(portName, 115200);
    
    if (fd > 0) {
        serial_set_attr(fd, 8, 'N', 1, 0);
        Logger(MLLogInfo, "<%s>: Success to connect the loadcell, port name: %s, fd: %d\n", __func__, portName, fd);
    } else {
        fd = -1;
        Logger(MLLogError, "<%s>: Fail to connect the loadcell, port name: %s\n", __func__, portName);
    }
    pthread_mutex_init(&lock, NULL);
    
    return fd;
}


int DisconnectXJCPort(int fd) {
    int flag = -1;
    
    if (fd > 0) {
        flag = serial_close(fd);
        Logger(MLLogInfo, "<%s>: Success to disconnect the loadcell, fd: %d\n", __func__, fd);
    }
    pthread_mutex_destroy(&lock);
    return flag;
}

char* ReadXJCValue(int fd) {
    int length = 128 * sizeof(char);
    char *buffer = (char *)malloc(length);
    char *substr = (char *)malloc(16);
    memset(substr, 0, 16);
    
    if (fd > 0) {
        int j = 0;
        
        if (serial_data_available(fd, 200)) {
            serial_receive(fd, buffer, length);
            
            int state = 0;
            
            for (int i = 0; i < strlen(buffer); i++) {
                if (buffer[0] != '=' && !state) {
                    state = 1;
                    continue;
                }
                
                if (buffer[i] == '\r') {
                    if (j != 0) { break;}
                    else { continue; }
                } else {
                    substr[j++] = buffer[i];
                }
            }
        }
    }
    free(buffer);
    
    Logger(MLLogInfo, "<%s>: Loadcell value: %s, fd: %d\n", __func__, substr, fd);
    
    return substr;
}

double ReadXJCExisting(int fd, char* endStr) {
    double value = -999;
    int size = 1024 * sizeof(char);
    
    int cnt = 0;
    
    if (fd > 0) {
        pthread_mutex_lock(&lock);
        
        do {
            XJCSend(fd, 0);
            usleep(10000);
//            int times = 0;
            char buffer[size];
            memset(buffer, 0, size);
            
            if (serial_data_available(fd, 100)) {
                // 至少读取到20个字符
//                do {
                char tmpBuf[size/4];
                memset(tmpBuf, 0, size/4);
                serial_receive(fd, tmpBuf, size);
//                    sprintf(buffer, "%s%s", buffer, tmpBuf);
                strcat(buffer, tmpBuf);
//                usleep(10000);
//                } while (strlen(buffer) <= 20 && times++ <= 3);
                
                int state = 0;
                int j = 0;
                char line[16];
                memset(line, 0, 16);
                
//                // 找出以‘=’开始的第最后一行
//                for (size_t i = strlen(buffer)-1; i >= 0; i--) {
//                    if (buffer[i] != '\r' && !state) {
//                        state = 1;
//                        continue;
//                    }
//
//                    if (buffer[i] == '=') {
//                        if (j != 0) {
//                            line[j++] = buffer[i];
//                            break;
//                        }
//                        else { continue; }
//                    } else {
//                        line[j++] = buffer[i];
//                    }
//                }
//
//                if (strlen(line) < 9) {
//                    break;
//                }
//
//                size_t lineCnt = strlen(line);
//                for (int k = 0; k < lineCnt / 2; k++) {
//                    char c = line[k];
//                    line[k] = line[lineCnt-k-1];
//                    line[lineCnt-k-1] = c;
//                }
                
                // 找出以‘=’开始的第一行
                for (int i = 0; i < strlen(buffer); i++) {
                    if (buffer[0] != '=' && !state) {
                        state = 1;
                        continue;
                    }

                    if (buffer[i] == '\r') {
                        if (j != 0) { break;}
                        else { continue; }
                    } else {
                        line[j++] = buffer[i];
                    }
                }
                
                // 找到 endmark之前的字符串
                char *subStr = strtok(line, endStr);
                
                if (subStr != NULL) {
                    size_t length = strlen(subStr);
                    
                    if (length > 2) {
                        char first = subStr[0];
                        char second = subStr[1];
                        char valueString[length+1];
                        memset(valueString, 0, length+1);
                        
                        // 转化字符串为数字
                        if (first == '=') {
                            switch (second) {
                                case '+':
                                case '-':
                                    for (int i = 1; i < length; i++) {
                                        valueString[i-1] = subStr[i];
                                    }
                                    value = atof(valueString);
                                    break;
                                case '.':
                                    value = atof(subStr);
                                    break;
                                default:
                                    value = -998;
                                    printf("loadcell value error.\n");
                                    break;
                            }
                        } else {
                            value = atof(subStr);
                        }
                        
                        break;
                    } else {
                        value = -998;   // not read value.
                        continue;
                    }
                }
                value = -997;   // not read value.
            }
        } while (cnt++ < 3);
    }
    pthread_mutex_unlock(&lock);
    
     Logger(MLLogInfo, "<%s>: Loadcell value: %lf, fd: %d\n", __func__, value, fd);
    
    return value;
}

int XJCSend(int fd, int mode) {
    if (fd > 0) {
        size_t size = 0;
        switch (mode) {
            case 0: {
                char *cmd = "#01\r\n";
                size_t length = strlen(cmd);
                return (int)serial_send(fd, cmd, length);
                }
            case 1: {
                char* cmd1 = "%0100+001111\r\n";
                char* cmd2 = "%014E+000001\r\n";
                size_t length1 = strlen(cmd1);
                size_t length2 = strlen(cmd2);
                size += serial_send(fd, cmd1, length1);
                usleep(20000);
                size += serial_send(fd, cmd2, length2);
                return (int)size;
                }
            case 2: {
                char *cmd = "%01@@2302+000000\r\n";
                size_t length = strlen(cmd);
                return (int)serial_send(fd, cmd, length);
            }
            default:
                return 0;
        }
    }
    
    return 0;
}

int OneByOneSend(int fd) {
    if (fd > 0) {
        char *cmd = "#01\r\n";
        size_t length = strlen(cmd);
        return (int)serial_send(fd, cmd, length);
    }
    
    return 0;
}

int AlwaysSend(int fd) {
    if (fd > 0) {
        char* com1 = "%0100+001111\r\n";
        char* com2 = "%014E+000001\r\n";
        serial_send(fd,com1,strlen(com1));
        usleep(1000);
        serial_send(fd,com2,strlen(com2));
    }

    return 0;
}

int StopSend(int fd) {
    if (fd > 0) {
        char *cmd = "STOP\r\n";
        return (int)serial_send(fd, cmd, strlen(cmd));
    }
    return 0;
}

int GoBackOneByOneSend(int fd) {
    if (fd > 0) {
        char *cmd = "%014E+000000\r\n";
        return (int)serial_send(fd, cmd, strlen(cmd));
    }
    return 0;
}

void ZeroXJC(int fd) {
    if (fd > 0) {
        char *cmd = "%01@@2302+000000\r\n";
        serial_send(fd, cmd, strlen(cmd));
        Logger(MLLogInfo, "<%s>: Zero loadcell value, fd: %d\n", __func__, fd);
    }
}
