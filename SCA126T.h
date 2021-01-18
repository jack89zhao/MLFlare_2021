//
//  SCA126T.h
//  FS1536
//
//  Created by ViKing Lai on 2019/7/30.
//  Copyright © 2019 ViKing Lai. All rights reserved.
//

#ifndef SCA126T_h
#define SCA126T_h

#include <stdio.h>

#endif /* SCA126T_h */

int ConnectSCAPort(char* portName);
int DisconnectSCAPort(void);
int SendXAxisCom(void);
int SendYAxisCom(void);
int SendXYAxisCom(void);
int SendZeroAxisCom(void);
int SendBaudAxisCom(void);
int SendOutputAxisCom(void);
int SendAddAxisCom(void);


int XYReadExistingHex(uint8_t* buffer,uint8_t* bufferRead, char* endStr);
int XorYBytetoHex(char* str,char*buf);
int XYBytetoHex(char* str,char*bufx,char*bufy,char*buft);
int XYBytetoHexAna(char* str,char*buf,int startLoc,int len);
