//
//  MLConfiger.c
//  MLFlare
//
//  Created by Jackie Wang on 2019/7/4.
//  Copyright © 2019 Jackie Wang. All rights reserved.
//

#include "MLConfiger.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

static char *gFilename = NULL;
int hasErrorCode = 0;

void SetIniFileName(char *filename) {
    if (gFilename != NULL) {
        free(gFilename);
        gFilename = NULL;
    }
    
    gFilename = (char*)malloc(256);
    
    if (filename != NULL) {
        strcpy(gFilename, filename);
        
        int error = access(gFilename, F_OK);
        
        if (error != 0) {
            FILE *fp = fopen(gFilename, "a+");
            fputs("// Ini version 1.0.\n\n", fp);
            fclose(fp);
        }
    }
}

bool GetStringValue(char *title, char *key, char *buffer) {
    FILE *fp;
    int flag = 0;
    char sTitle[64], sLine[1024];
    char *wTmp;
    
    sprintf(sTitle, "[%s]", title);
    
    if (NULL == (fp = fopen(gFilename, "r"))) {
        perror("fail to open config file.");
        hasErrorCode = 1;
        return false;
    }
    
    while (NULL != fgets(sLine, 1024, fp)) {
        if (0 == strncmp("//", sLine, 2)) { continue; }     // 注释行
        if ('#' == sLine[0]) { continue; }                  // 注释行
        
        wTmp = strchr(sLine, '=');
        
        if ((NULL != wTmp) && (1 == flag)) {
            if (0 == strncmp(key, sLine, strlen(key))) {    // 长度以文件读取为准
                sLine[strlen(sLine)-1] = '\0';
                fclose(fp);
                
                while (*(wTmp+1) == ' ') {
                    wTmp++;
                }
                strcpy(buffer, wTmp+1);
                return true;
            }
        } else {
            if (0 == strncmp(sTitle, sLine, strlen(sTitle))) {
                flag = 1;   // 找到标题位置
            }
        }
    }
    fclose(fp);
    
    hasErrorCode = 2;
    return false;
}

int GetIntValue(char *title, char *key) {
    int flag;
    char *buffer = (char *)malloc(32);
    int result = INT_MIN;
    flag = GetStringValue(title, key, buffer);
    
    if (flag) {
        result = atoi(buffer);
    }
    
    return result;
}

long GetLongValue(char *title, char *key) {
    char *buffer = (char *)malloc(64);
    long result = LONG_MIN;
    if (GetStringValue(title, key, buffer)) {
        result = atol(buffer);
    }
    
    return result;
}

double GetDoubleValue(char *title, char *key) {
    char *buffer = (char *)malloc(64);
    double result = LONG_MIN;
    if (GetStringValue(title, key, buffer)) {
        result = atof(buffer);
    }
    
    return result;
}

int checkTitle(char *title)
{
    FILE *fpr, *fpw;
    int  flag = 0;
    char sLine[1024], sTitle[32];
    sprintf(sTitle, "[%s]", title);
    if (NULL == (fpr = fopen(gFilename, "r")))
        return -1;
    sprintf(sLine, "%s.tmp", gFilename);
    if (NULL == (fpw = fopen(sLine,    "w")))
        return -1;
    while (NULL != fgets(sLine, 1024, fpr)) {
        if (2 != flag) {
            {
                if (0 == strncmp(sTitle, sLine, strlen(sTitle))) {
                    flag = 1;
                }
            }
        }
        fputs(sLine, fpw);
    }
    fclose(fpr);
    fclose(fpw);
    sprintf(sLine, "%s.tmp", gFilename);
    return flag;
}



int ModifyKeyString(char *title,char *val)
{
    FILE *fpr, *fpw;
    int  flag = 0;
    char sLine[1024], sTitle[32];
    sprintf(sTitle, "[%s]", title);
    if (NULL == (fpr = fopen(gFilename, "r")))
        return -1;
    sprintf(sLine, "%s.tmp", gFilename);
    if (NULL == (fpw = fopen(sLine,    "w")))
        return -1;
    while (NULL != fgets(sLine, 1024, fpr)) {
        if (2 != flag) {
            {
                if (0 == strncmp(sTitle, sLine, strlen(sTitle))) {
                    sprintf(sLine, "[%s]\n", val);
                    
                    flag = 1;
                }
            }
        }
        fputs(sLine, fpw);
    }
    fclose(fpr);
    fclose(fpw);
    sprintf(sLine, "%s.tmp", gFilename);
    return rename(sLine, gFilename);
}


bool PutStringValue(char *title, char *key, char *value) {
    return InsertStringValue(title, NULL, key, value);
}

bool PutIntValue(char *title, char *key, int value) {
    char val[32];
    sprintf(val, "%d", value);
    return InsertStringValue(title, NULL, key, val);
}

bool PutLongValue(char *title, char *key, long value) {
    char val[64];
    sprintf(val, "%ld", value);
    return InsertStringValue(title, NULL, key, val);
}

bool PutFloatValue(char *title, char *key, float value) {
    char val[32];
    sprintf(val, "%f", value);
    return InsertStringValue(title, NULL, key, val);    // PutStringValue(title, key, val);
}

bool PutDoubleValue(char *title, char *key, double value) {
    char val[64];
    sprintf(val, "%lf", value);
    return InsertStringValue(title, NULL, key, val);    // PutStringValue(title, key, val);
}

bool InsertStringValue(char *title, char *searchKey, char *key, char *value) {
    FILE *fpr, *fpw;
    int flag = 0;
    bool find = false;
    bool isNewKey = true;
    char sLine[1024], sTitle[32], temp[1024];
    char *wTmp;
    
    assert(title != NULL);
    assert(key != NULL);
    
    sprintf(sTitle, "[%s]", title);
    
    if (NULL == (fpr=fopen(gFilename, "r"))) {
        hasErrorCode = 1;
        return false;
    }
    
    sprintf(sLine, "%s.tmp", gFilename);
    
    if (NULL == (fpw=fopen(sLine, "w+"))) {
        hasErrorCode = 1;
        return false;
    }
    
    while (NULL != fgets(sLine, 1024, fpr)) {
        if (find && (0 == strncmp("[", sLine, strlen("[")))) {
            find = false;
        }
        if (0 == strncmp(sTitle, sLine, strlen(sTitle))) {
            find = true;
        }
        
        if (find) {
            if (searchKey != NULL) {
                if (0 == strncmp(searchKey, sLine, strlen(searchKey))) {
                    flag = 1;
                }
                
                if (flag == 1) {
                    wTmp = strchr(sLine, '=');
                    if (NULL == value) {
                        value = "//";
                    }
                    if (NULL != wTmp) {
                        sprintf(temp, "%s%s = %s\n", sLine, key, value);
                        fputs(temp, fpw);
                        break;
                    }
                }
            } else {
                flag = isNewKey ? 2 : 3;    // 3 is old key.
            }
            
            if (0 == strncmp(key, sLine, strlen(key))) {
                if (NULL == value) {
                    value = "//";
                }
                sprintf(temp, "%s = %s\n", key, value);
                fputs(temp, fpw);
                isNewKey = false;
                flag = 3;   // if current line is last line, need set flag = 3 to igore old key.
                continue;
            }
        }
        fputs(sLine, fpw);
    }
    
    switch (flag) {
        case 0:
            if (find) { // find title.
                sprintf(temp, "%s = %s\n", key, value);
                fputs(temp, fpw);
            } else {    // not find title.
                sprintf(temp, "\n%s\n", sTitle);
                fputs(temp, fpw);
                sprintf(temp, "%s = %s\n", key, value);
                fputs(temp, fpw);
            }
            break;
        case 2:
            sprintf(temp, "%s = %s\n", key, value);
            fputs(temp, fpw);
            break;
        default:
            break;
    }
    
    fclose(fpr);
    fclose(fpw);
    sprintf(sLine, "%s.tmp", gFilename);
    
    int yesno = rename(sLine, gFilename);    // 将临时文件更新到原文件
    hasErrorCode = (yesno == 0) ? 0 : 2;
    return (yesno == 0) ? true : false;
}

char *GetConfigFileErrorMessage() {
    if (hasErrorCode == 2) {
        hasErrorCode = 0;
        return "Empty profile file";
    }
    else if (hasErrorCode == 1) {
        hasErrorCode = 0;
        return strerror(errno);
    } else {
        hasErrorCode = 0;
        return "";
    }
}
