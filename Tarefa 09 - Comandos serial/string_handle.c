/*
 * string_handle.c
 *
 *  Created on: 20 de ago de 2021
 *      Author: Nycolas
 */

#include "string_handle.h"

void removeSubstr(char *string, char *sub)
{
    char *match;
    int len = strlen(sub);
    while ((match = strstr(string, sub)))
    {
        *match = '\0';
        strcat(string, match + len);
    }

}

void clearstring(char *string)
{
    memset(string, '\0', sizeof(string));
    //memset(string,'\0',strlen(string));
}
