//
//  JKCL200A.h
//  CL200A
//
//  Created by Jackie Wang on 2019/10/19.
//  Copyright © 2019 mt-007. All rights reserved.
//

#ifndef JKCL200A_h
#define JKCL200A_h

#include <stdio.h>

/* Connect the CL200A serial port, return 0: connected, -1: failure */
int JKConnectCL200A(const char* portname, const char *receptor);

/* Close the CL200A serial port */
int JKDisconnectCL200A(void);

/* Clear the cache */
void JKClearBuffer(void);

/* set receptor value */
void JKSetReceptor(const char* receptor);

/* Measurement data can be read X, Y, Z values */
void JKGetCL200AXYZ (float* x,  float* y, float* z);

/* Measurement data can be read EV, x', y' values */
void JKGetCL200AEvXY(float * Ev, float * x, float * y);

/* Measurement data can be read EV, u', v' values */
void JKGetCL200AEvUV(float* Ev, float* u, float* v);

/* Measurement data can be read X, Y, Z values; EV, x', y' values; EV, u', v' values */
void JKGetCL200AALL(float* XYZArray, float* EvXYArray, float* EvUVArray);

void JKSetCL200ATimeout(int timeout);

#endif /* JKCL200A_h */
