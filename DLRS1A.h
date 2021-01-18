//
//  DLRS1A.h
//  serial
//
//  Created by Kerwin on 2019/7/16.
//  Copyright © 2019 Kerwin. All rights reserved.
//

#ifndef DLRS1A_h
#define DLRS1A_h

#include <stdio.h>
#include <stdbool.h>

bool ConnectDLRS1A(char *portname);
void DisconnectDLRS1A(void);
double GetDLRS1AMeasureValue(void);
double GetDLRS1AM0MeasureValue(void);
char* GetDLRS1AAllData(void);

#endif /* DLRS1A_h */
