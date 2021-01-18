//
//  SCA126T.c
//  FS1536
//
//  Created by ViKing Lai on 2019/7/30.
//  Copyright © 2019 ViKing Lai. All rights reserved.
//

#include "SCA126T.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "serial.h"
#include <string.h>
#include <ctype.h>

static serial_port_t port;

int ConnectSCAPort(char* portName) {
    return open_serial_port(portName, 9600, SETTINGS_DEFAULT, &port);
}


int DisconnectSCAPort() {
    return close_serial_port(&port);
}


int ReadSCAExisting(uint8_t* buffer, char* endStr) {
    
    return serial_get_line_endMark(&port, buffer, 1024, endStr, 3000);
}

int Hex2Str( uint8_t *sSrc,  uint8_t *sDest, int nSrcLen )
{
    int  i;
    char szTmp[3];
    
    for( i = 0; i < nSrcLen; i++ )
    {
        sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );
        memcpy( &sDest[i * 2], szTmp, 2 );
    }
    return 0;
}

int XYReadExistingHex(uint8_t* buffer,uint8_t* bufferRead, char* endStr) {
    
    byte_fifo_copy_endMark(&port.rx_buffer,buffer,1024,endStr);
    
    return Hex2Str(buffer, bufferRead, 14);
}

int XYBytetoHexAna(char* str,char*buf,int startLoc,int len)
{
    int n, m = 0;
    char arr[32];
    double val = 0;
    int i = 0;
    char subdata[32];
    char* data = str;
    char* dest_str = (char*)malloc(1024);
    char* ret_str = (char*)malloc(1024);
    while(len) {
        subdata[i] = data[startLoc + i];
        i++;
        len--;
    }
    for (n = 1; n < 6; n++) {
        arr[m++] = subdata[n];
    }
    sprintf(dest_str, "%s", arr);
    val = atof(dest_str)/100;
    
    if (subdata[0]=='1') {
        val = val*(-1);
    }
    sprintf(ret_str, "%f", val);
    
    strcpy(buf,ret_str);
    
    return 0;
}

int XYBytetoHex(char* str,char*bufx,char*bufy,char*buft)
{
    
    XYBytetoHexAna(str, bufx, 8, 6);
    XYBytetoHexAna(str, bufy, 14, 6);
    XYBytetoHexAna(str, buft, 20, 6);

    return 0;
}


#define C2I(c) ((c >= '0' && c<='9') ? (c-'0') : ((c >= 'a' && c <= 'z') ? (c - 'a' + 10): ((c >= 'A' && c <= 'Z')?(c - 'A' + 10):(-1))))

char* HexConvertToBytes(char*str)
{
    int count = (uint32_t)strlen(str);
    int8_t bytes[count / 2];
    for(int i = 0; i<count; i+=2)
    {
        char c1 = *(str + i);
        char c2 = *(str + i + 1);
        if(C2I(c1) >= 0 && C2I(c2) >= 0)
        {
            bytes[i / 2] = C2I(c1) * 16 + C2I(c2);
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
    
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);
        
        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;
        
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        
        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
}

int Change(char s[],char bits[]) {
    int i,n = 0;
    for(i = 0; s[i]; i += 2) {
        if(s[i] >= 'A' && s[i] <= 'F')
            bits[n] = s[i] - 'A' + 10;
        else bits[n] = s[i] - '0';
        if(s[i + 1] >= 'A' && s[i + 1] <= 'F')
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
        ++n;
    }
    return n;
}

//check x angle
int SendXAxisCom(){
    char s[] = "6804000105";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
}

//check y angle
int SendYAxisCom(){
    char s[] = "6804000206";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
    
}

//check xy angle
int SendXYAxisCom(){
    char s[] = "6804000408";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
}

//return to zero
int SendZeroAxisCom(){
    char s[] = "68050005000A";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
}

//set baud rate
int SendBaudAxisCom(){
    char s[] = "6805000B0313";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
}

//set output mode
int SendOutputAxisCom(){
    char s[] = "6805000C0011";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
}

//set address
int SendAddAxisCom(){
    char s[] = "6805FF0F0013";
    char bits[10];
    int n = Change(s,bits);
    
    serial_write_Hex(&port,bits,n);
    sleep(1);
    return 0;
}

int analyResult(char* str)
{
    int i = 0;
    char subdata[32];
    char* data = str;
    int startLoc = 4;
    int len = 2;
    
    while(len) {
        subdata[i] = data[startLoc + i];
        i++;
        len--;
    }
    
    if (subdata[0]=='F') {
        return -1;
    }
    
    return 0;
}
