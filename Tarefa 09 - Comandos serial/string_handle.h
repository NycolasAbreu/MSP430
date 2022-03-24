/*
 * string_handle.h
 *
 *  Created on: 20 de ago de 2021
 *      Author: Nycolas
 */

#ifndef STRING_HANDLE_H_
#define STRING_HANDLE_H_

#include <string.h>
#include <msp430.h>

void removeSubstr(char *string, char *sub);
void clearstring(char *string);

#endif /* STRING_HANDLE_H_ */
