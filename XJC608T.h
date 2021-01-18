//
//  XJC608T.h
//  FS1536
//
//  Created by ViKing Lai on 2019/7/20.
//  Copyright © 2019 ViKing Lai. All rights reserved.
//

#ifndef XJC608T_h
#define XJC608T_h

#include <stdio.h>

#endif /* XJC608T_h */
int ConnectXJCPort(const char* portName);
int DisconnectXJCPort(int fd);
double ReadXJCExisting(int fd, char* endStr);
char* ReadXJCValue(int fd);
int XJCSend(int fd, int mode);

int OneByOneSend(int fd);
int StopSend(int fd);
int AlwaysSend(int fd);
int GoBackOneByOneSend(int fd);
void ZeroXJC(int fd);
