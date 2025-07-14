/*
 * bsp.h
 *
 *  Created on: Apr 12, 2025
 *      Author: cikkr
 */

#ifndef USER_BSP_H_
#define USER_BSP_H_


#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"


#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"


#define MULTI_TENSION  	IN1_Pin
#define MULTI_TENSION2  IN2_Pin
#define WIRE_STATUS    	OUT0_Pin

#define ArraySize				8

/*Fnd*/
#define FND1_CLK				1
#define FND2_CLK				2
#define FND3_CLK				4
#define LED_CLK					8
#define FND_nOE					16

#define SetSaveCnt			50

#define FlickerTimeOn		100
#define FlickerTimeOff	50


#define ENCODER_STOP 		0
#define ENCODER_RUN 		1

#define OFF			0
#define ON			1

#define CCW 		0
#define CW 			1


#define DOWN 		0
#define UP 			1

#define FLICKER_OFF 0
#define FLICKER_ON 1

#define EDIT_OFF 0
#define EDIT_ON 1


#define CmdWriteEeprom 	0
#define CmdReadEeprom 	1
#define CmdReadVariable 2


/*Run Mode Stuats*/
#define RUN				0
#define EDIT			1
#define EDIT_DATA		2
#define CAL_ADJUST		3

/*Button Status*/
#define ENCODER_BUTTON  1
#define KEY_RUNEDIT			2 	 //Mode
#define KEY_SET 		    4
#define KEY_RESET	    	6	//RUNEDIT + Set

#define KeyInitStatus	0
#define KeyDownStatus	1
#define KeyUpStatus		2

#define SaveTime		10
#define KeyDelay		5
#define AUTO_COUNT		2 //��ư�� ������ ������ �ڵ����� �߰�
#define EncoderTimeOut	100


#define SET_VALUE		0
#define REAL_VALUE		1

/*Led */
#define DOWN 			0
#define UP 				1
#define RUNNING			2


/*Parameter List*/
#define PAR_MODEL							1		// model
#define PAR_CHK_ANG						2		// wire check angle
#define PAR_P									3
#define PAR_I									4
#define PAR_D									5
#define PAR_MAX								6
#define PAR_MIN								7

#define PAR_POS								8		//
#define PAR_DISP							9		//0 tension, 1 volt tension

#define PAR_MULTI							10  // 2�� �ټ�
#define PAR_LOADCELL_OFFSET		11 	// load cell offset PAR_LOADCELL
#define PAR_ADGAIN						12

#define PAR_PLIMIT						13
#define PAR_MLIMIT						14
#define PAR_LOADCELL_SPAN_L		15  // 100 Load cell span  PAR_OFFSETA
#define PAR_LOADCELL_SPAN_H		16  // 500


#define PAR_BackupModel			17
#define PAR_fAdcValue			18

/* Private variables */
#define DOT				10
#define BAR				11


#define epsilon 0.01
#define dt 1.0             //100ms loo;p time
#define MAX  250
#define MIN -250
#define Kp  1.0
#define Kd  0.1	//0.1
#define Ki  0.9	//0.9
#define PI  3.14159265358979f


/* Private function prototypes */
void Delay_us(uint16_t time_us);			// time delay for us in 72MHz
void Delay_ms(uint16_t time_ms);			// time delay for ms in 72MHz

void FndDisplay(float output_value,uint8_t digit );
void FndDisplayString(uint8_t strValue);
void FndDispalyParameter(uint8_t Parameter);


void FndShadowOutput(uint8_t fndPos ,uint8_t fndVlaue);
void FndOutput(uint8_t fndPos ,uint8_t fndVlaue);

void FndAllOff(void);
void FndAllOn(void);

void FndDisplayShadow(void);
void FndFlicker(uint8_t onOff);

void PannelLed(uint8_t ledPos,uint8_t onOff);
void PannelLEdFlicker(uint8_t ledPos,uint8_t onOff);
void PannelLEdAll(uint8_t onOff);
void FndIncrementBar(uint8_t fndPos);


uint8_t getPbVal(void);
GPIO_PinState GetInput(uint16_t input);
void SetOutput(uint16_t output,uint8_t status);

float PIDcal(float setpoint,float actual_position);

#endif /* USER_BSP_H_ */
