//
//  CL200A.h
//  CL200A
//
//  Created by mt-007 on 2019/7/10.
//  Copyright © 2019 mt-007. All rights reserved.
//
/**
 
 */

#ifndef CL200A_h
#define CL200A_h

#include <stdio.h>

/* Connect the CL200A serial port, return 0: connected, -1: failure */
int ConnectCL200A(const char* portname, const char *receptor);

/* Close the CL200A serial port */
int DisconnectCL200A(void);

/* Clear the cache */
void ClearBuffer(void);

/* set receptor value */
void SetReceptor(const char* receptor);

/* Measurement data can be read X, Y, Z values */
void GetCL200AXYZ (float* x,  float* y, float* z);

/* Measurement data can be read EV, x', y' values */
void GetCL200AEvXY(float * Ev, float * x, float * y);

/* Measurement data can be read EV, u', v' values */
void GetCL200AEvUV(float* Ev, float* u, float* v);

/* Measurement data can be read X, Y, Z values; EV, x', y' values; EV, u', v' values */
void GetCL200AALL(float* XYZArray, float* EvXYArray, float* EvUVArray);

void SetCL200ATimeout(int timeout);

#endif /* CL200A_h */


