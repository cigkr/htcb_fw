/*
 * app.h
 *
 *  Created on: Apr 12, 2025
 *      Author: cikkr
 */

#ifndef USER_AP_H_
#define USER_AP_H_


#include "bsp.h"
#include "eeprom.h"



void UpDateEncoder(void);
void DataManager(uint8_t WhatCmd,uint8_t modelNumber,uint8_t parameter);
void LoadParameter(uint8_t modelNumber);
void ResetParameter(uint8_t modelNum);
int8_t ParamterSet(uint8_t prmt);

#endif /* USER_AP_H_ */
