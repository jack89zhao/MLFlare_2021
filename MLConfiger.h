//
//  MLConfiger.h
//  MLFlare
//
//  Created by Jackie Wang on 2019/7/4.
//  Copyright © 2019 Jackie Wang. All rights reserved.
//

#ifndef MLConfiger_h
#define MLConfiger_h

#include <stdio.h>
#include <stdbool.h>

// set file name.
void SetIniFileName(char *filename);
// get string value with title and key.
bool GetStringValue(char *title, char *key, char *buffer);
// get int value with title and key.
int GetIntValue(char *title, char *key);
// get long value with title and key.
long GetLongValue(char *title, char *key);
// get double value with title and key.
double GetDoubleValue(char *title, char *key);

// set string value with title and key.
bool PutStringValue(char *title, char *key, char *value);
// set int value with title and key.
bool PutIntValue(char *title, char *key, int value);
// set long value with title and key.
bool PutLongValue(char *title, char *key, long value);
// set float value with title and key.
bool PutFloatValue(char *title, char *key, float value);
// set double value with title and key.
bool PutDoubleValue(char *title, char *key, double value);

bool InsertStringValue(char *title, char *searchKey, char *key, char *value);

char *GetConfigFileErrorMessage(void);

int ModifyKeyString(char *title,char *val);
int checkTitle(char *title);
#endif /* MLConfiger_h */
