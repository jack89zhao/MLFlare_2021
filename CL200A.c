//
//  CL200A.c
//  CL200A
//
//  Created by mt-007 on 2019/7/10.
//  Copyright © 2019 mt-007. All rights reserved.
//

#include "CL200A.h"
#include "serial.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static serial_port_t port;
static char *g_receptor = NULL;
static int is_connected = 1;
static int gTimeout;

int fun(char *s,char *t);
int GetBcc(char command[]);
int WriteCmd(const char* header, const char* cmd, uint32_t size);
char IntConvertToChar(int value);
int CharConvertToInt(char* value);
int GetBuffer(char buffer[]);
float CheckEnd(char character);
float ParserData(char* data);
float GetSub(char buffer[], int startLoc, int len);
void GetCommand(char* cmd, char* command);
void SubStr(char* subdata, char data[], int startLoc, int len);

void SetCL200ATimeout(int timeout) {
    if (timeout > 0) {
        gTimeout = timeout;
    }
}

int ConnectCL200A(const char* portname, const char *receptor) {
    is_connected = open_serial_port(portname, 9600, SETTINGS_SEVEN_BITS_EVEN_PARITY, &port);
    if (!is_connected) {
        SetReceptor(receptor);
    }
    
    return is_connected;
}

int DisconnectCL200A(void) {
    
    if (is_connected != 0) { return 0; }
    
    if (g_receptor != NULL) {
        free(g_receptor);
        g_receptor = NULL;
    }
    
    is_connected = -1;
    
    return close_serial_port(&port);
}

int GetBuffer(char buffer[]) {
    
    uint32_t size = 100;
    if (is_connected != 0) { return 0; }
    
    return serial_get_line(&port, (uint8_t*)buffer, size, 500);
}

void SetReceptor(const char* receptor) {
    
    if (is_connected != 0) { return; }
    
    if (g_receptor != NULL) {
        free(g_receptor);
        g_receptor = NULL;
    }
    
    g_receptor = malloc(strlen(receptor) * sizeof(char));
    strcpy(g_receptor,receptor);
    
    char buffer[128]={};
    uint32_t size = 14;
    
    WriteCmd(g_receptor, "541   ", size);
    usleep(500000);
    GetBuffer(buffer);
    
    WriteCmd("", "99551  0", size);
    usleep(175000);
    
    WriteCmd(g_receptor, "4010  ", size);
    usleep(500000);
    GetBuffer(buffer);
}

void GetCL200AXYZ(float* x, float* y, float* z) {
    
    if (is_connected != 0) { return ; }
    
    char buffer[128]={};
    uint32_t size = 14;
    
    WriteCmd("", "994021  ", size);
    usleep(500000);
    
    WriteCmd(g_receptor, "011200", size);
    usleep(500000);
    GetBuffer(buffer);
    *x = GetSub(buffer, 9,  6);
    *y  = GetSub(buffer, 15, 6);
    *z  = GetSub(buffer, 21, 6);
}

void GetCL200AEvXY(float * Ev, float * x, float * y) {
    
    if (is_connected != 0) { return; }
    
    char buffer[128]={};
    uint32_t size = 14;
    
    WriteCmd("", "994021  ", size);
    sleep(1);

    WriteCmd(g_receptor, "021200", size);
    sleep(1);
    GetBuffer(buffer);
    *Ev = GetSub(buffer, 9,  6);
    *x  = GetSub(buffer, 15, 6);
    *y  = GetSub(buffer, 21, 6);
}

void GetCL200AEvUV(float* Ev, float* u, float* v) {
    
    if (is_connected != 0) { return ; }
    
    char buffer[128]={};
    uint32_t size = 14;
    
    WriteCmd("", "994021  ", size);
    usleep(500000);
    
    WriteCmd(g_receptor, "031200", size);
    usleep(500000);
    GetBuffer(buffer);
    *Ev = GetSub(buffer, 9,  6);
    *u  = GetSub(buffer, 15, 6);
    *v  = GetSub(buffer, 21, 6);
}

void GetCL200AALL(float* XYZArray, float* EvXYArray, float* EvUVArray) {
    if (is_connected != 0) { return ; }
    
    char buffer[128]={};
    uint32_t size = 14;
    
    // take sample data
    WriteCmd("", "994021  ", size);
    usleep(500000);
    WriteCmd(g_receptor,"011200", size);
    usleep(500000);
    GetBuffer(buffer);
    XYZArray[0] = GetSub(buffer, 9,  6);
    XYZArray[1] = GetSub(buffer, 15, 6);
    XYZArray[2] = GetSub(buffer, 21, 6);
    
    WriteCmd(g_receptor, "021200", size);
    usleep(500000);
    GetBuffer(buffer);
    EvXYArray[0] = GetSub(buffer, 9,  6);
    EvXYArray[1] = GetSub(buffer, 15, 6);
    EvXYArray[2] = GetSub(buffer, 21, 6);
    
    WriteCmd(g_receptor, "031200", size);
    usleep(500000);
    GetBuffer(buffer);
    EvUVArray[0] = GetSub(buffer, 9,  6);
    EvUVArray[1] = GetSub(buffer, 15, 6);
    EvUVArray[2] = GetSub(buffer, 21, 6);
}

void ClearBuffer(void) {
    
    if (is_connected != 0) { return ; }
    
    char buffer[128]={};
    uint32_t size = 14;
    
    WriteCmd("", "994021  ", size);
    usleep(500000);
    
    WriteCmd(g_receptor, "011200", size);
    usleep(500000);
    GetBuffer(buffer);
}

int WriteCmd(const char* header, const char* cmd, uint32_t size) {

    char *cmdTmp = (char *)malloc(9 * sizeof(char));
    char command[14] = {};
    
    if (strcmp(header, "") != 0) {
        strcat(cmdTmp, header);
    }
    
    strcat(cmdTmp, cmd);
    GetCommand(cmdTmp,command);
    
    int write = serial_write_string(&port, (uint8_t*)command, size);
    free(cmdTmp);
    
    return write;
}

int GetBcc(char command[]) {
    
    int BCC;
    BCC = command[0];
    for (int i=1; i<8; i++) {
        BCC^= command[i];
    }
    BCC^=0x03;
    
    return BCC;
}

char IntConvertToChar(int value) {
    
    char val;
    if (value<0 || value>15)
        return '\0';
    if (value<10)
        val='0'+value;
    else
        val='A'+(value-10);
    return val;
}

float GetSub(char buffer[],int startLoc, int len) {
    
    char v[6];
    char symbol[1];
    SubStr(v, buffer, startLoc,  len);
    float value = ParserData(v);
    
    SubStr( symbol, v, 0,  1 );
    int equal1 = fun(symbol, "+");
    int equal2 = fun(symbol, "=");
    int equal3 = fun(symbol, "-");
    
    if (equal1 || equal2) {
        value *= 1;
    }
    if (equal3) {
        value *= -1;
    }
    
    return value;
}

/* Whether the two strings are the same */
int fun(char *ch1,char *ch2)
{
    int m = 0;
    while(*(ch1+m)==*(ch2+m)&&*(ch2+m)!='\0'&&*(ch1+m)!='\0')
    {
        m++;
    }
    if(*(ch2+m)!='\0'||*(ch1+m)!='\0')
    {
        return 0; //Ch1 is different as ch2
    }
    
    return 1;
}

float ParserData(char* data) {
    
    char subdata[5];
    SubStr(subdata, data, 1,  4);
    
    char character = data[5];
    float endExp = CheckEnd(character);
    int tmpdata = CharConvertToInt(subdata);
    float value = tmpdata * endExp;
    
    return value;
}

float CheckEnd(char character) {
    
    double value = 0.0;
    
    switch (character) {
        case '0': value = pow(10, -4); break;
        case '1': value = pow(10, -3); break;
        case '2': value = pow(10, -2); break;
        case '3': value = pow(10, -1); break;
        case '4': value = pow(10, 0);  break;
        case '5': value = pow(10, 1);  break;
        case '6': value = pow(10, 2);  break;
        case '7': value = pow(10, 3);  break;
        case '8': value = pow(10, 4);  break;
        case '9': value = pow(10, 5);  break;
        default : break;
    }
    
    return (float)value;
}

int CharConvertToInt(char* value) {
    
    int oneValue = 0;
    int twoValue = 0;
    int threeValue = 0;
    int fourValue = 0;
    
    for(int i=0;i<strlen(value);i++) {
        char temp_char=value[i];
        
        int temp = 0;
        if (temp_char>='0') {
            int temp_int=temp_char-'0';
            temp = temp_int * pow(10, strlen(value)-i-1);
        }
        
        if (i==0) {
            oneValue = temp;
        }
        else if (i==1) {
            twoValue = temp;
        }
        else if (i==2) {
            threeValue = temp;
        }
        else if (i==3) {
            fourValue = temp;
        }
    }
    
    int tmpdata = oneValue + twoValue + threeValue + fourValue;
    
    return tmpdata;
}


void SubStr(char* subdata, char* data, int startLoc, int len) {
    
    assert(subdata);
    assert(data);
    char *p = data + startLoc;
    
    size_t n = strlen(p);
    int i = 0;
    if (n < len || startLoc < 0) {
        len = 0;
    }
    
    while(len) {
        subdata[i] = data[startLoc + i];
        i++;
        len--;
    }
    subdata[i] = '\0';
}

void GetCommand(char* cmd, char* command) {
    
    for (int i=0; i<14; i++) {
        
        char value = '\0';
        if (i==0) {
            command[0] = 0x02;
        }
        else if(i>=1&&i<=8) {
            value = cmd[i-1];
        }
        else if (i==9) {
            command[9] = 0x03;
        }
        else if (i==10) {
            int bcc = GetBcc(cmd);
            int v = bcc/16;
            value = IntConvertToChar(v);
        }
        else if (i==11) {
            int bcc = GetBcc(cmd);
            int v = bcc%16;
            value = IntConvertToChar(v);
        }
        else if (i==12) {
            command[12] = 0x0D;
        }
        else if (i==13) {
            command[13] = 0x0A;
        }
        
        switch (value) {
            case '0':
                command[i] = 0x30; continue;
            case '1':
                command[i] = 0x31; continue;
            case '2':
                command[i] = 0x32; continue;
            case '3':
                command[i] = 0x33; continue;
            case '4':
                command[i] = 0x34; continue;
            case '5':
                command[i] = 0x35; continue;
            case '6':
                command[i] = 0x36; continue;
            case '7':
                command[i] = 0x37; continue;
            case '8':
                command[i] = 0x38; continue;
            case '9':
                command[i] = 0x39; continue;
            case ' ':
                command[i] = 0x20; continue;
            default: break;
        }
    }
}
