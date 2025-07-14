/*
 * bsp.c
 *
 *  Created on: Apr 12, 2025
 *      Author: cikkr
 */

#include "bsp.h"



char FndTable[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x83,0xf8,0x80,0x98,0x7f,0xbf};
volatile uint8_t ShadowFnd1,ShadowFnd2,ShadowFnd3,ShadowLed;



/*
**********************************************************************************

**********************************************************************************
*/

/**
  * @brief Delay_us
  * @param uint16_t time_us
  * @retval	 None
  */
void Delay_us(uint16_t time_us)			/* time delay for us in 72MHz */
{
  register uint16_t i;
  for(i = 0; i < time_us; i++)
 { asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 5
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 10
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 15
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 20
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 25
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 30
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 35
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 40
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 45
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 50
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 55
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 60
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 65
      asm("NOP");				// 66
    }
}

/**
  * @brief Delay_ms
  * @param uint16_t time_ms
  * @retval	 None
  */
void Delay_ms(uint16_t time_ms)			/* time delay for ms in 72MHz */
{
  register uint16_t i;
  for(i = 0; i < time_ms; i++)
    Delay_us(1000);
}

/**
  * @brief FndDisplay
  * @param float output_value
  * @retval	 None
  */
void FndDisplay(float output_value,uint8_t digit ){
char source_buf[10];
char dest_buf[4];
uint8_t display_value=0;

	if(digit==0){
		sprintf(source_buf,"%04.1f",output_value);
		//printf("source_buf=%s\n",source_buf);
		if(output_value >100){
			//  *1 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+2), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] & FndTable[DOT]);
			//printf("fnd digit 1=%d\n",display_value);

			//  *10 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+1), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,(FndTable[display_value]));
			//printf("fnd digit 10=%d\n",display_value);

			// *100 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND1_CLK ,FndTable[display_value]);
			//printf("fnd digit 100=%d\n",display_value);
		}else if(output_value <100 && output_value >9){
			//  *1 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+3), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] );
			//printf("fnd digit 1=%d\n",display_value);

			//  *10 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+1), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,FndTable[display_value] & FndTable[DOT]);
			//printf("fnd digit 10=%d\n",display_value);

			// *100 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND1_CLK ,FndTable[display_value]);
			//printf("fnd digit 100=%d\n",display_value);
		}else{
			//  *1 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+4), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] );
			//printf("fnd digit 1=%d\n",display_value);

			//  *10 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+3), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,FndTable[display_value] );
			//printf("fnd digit 10=%d\n",display_value);

			// *100 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND1_CLK ,FndTable[display_value]& FndTable[DOT]);
			//printf("fnd digit 100=%d\n",display_value);
		}
	}else if (digit==1){
		sprintf(source_buf,"%04.1f",output_value);
		//printf("source_buf=%s\n",source_buf);
		//�Ҽ��� ���� ù��° �ڸ�
		memset(dest_buf,0,sizeof(dest_buf));
		strncpy(dest_buf, (source_buf+3), 1);
		display_value=atoi(dest_buf);
		FndShadowOutput(FND3_CLK ,FndTable[display_value]);
		//printf("fnd digit 10=%d\n",display_value);

		//  *1 ����+�Ҽ���
		memset(dest_buf,0,sizeof(dest_buf));
		strncpy(dest_buf, (source_buf+1), 1);
		display_value=atoi(dest_buf);
		FndShadowOutput(FND2_CLK ,(FndTable[display_value] & FndTable[DOT]));
		//printf("fnd digit 1=%d\n",display_value);

		// *10 ����
		memset(dest_buf,0,sizeof(dest_buf));
		strncpy(dest_buf, (source_buf+0), 1);
		display_value=atoi(dest_buf);
		FndShadowOutput(FND1_CLK ,FndTable[display_value]);
		//printf("fnd digit 0.1=%d\n",display_value);
	}else if (digit==2){
		sprintf(source_buf,"%04.2f",output_value);
		//printf("source_buf=%s\n",source_buf);

		 if(output_value >9.99){
			//  �Ҽ���2����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+3), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] );
			//printf("fnd digit 10=%d\n",display_value);

			//  �Ҽ���1����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+1), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,(FndTable[display_value])& FndTable[DOT]);
			//printf("fnd digit 1=%d\n",display_value);

			// *1 ����+�Ҽ���
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND1_CLK ,FndTable[display_value]);
		}else{
			//  �Ҽ���2����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+3), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] );
			//printf("fnd digit 10=%d\n",display_value);

			//  �Ҽ���1����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+2), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,(FndTable[display_value]));
			//printf("fnd digit 1=%d\n",display_value);

			// *1 ����+�Ҽ���
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND1_CLK ,FndTable[display_value]& FndTable[DOT]);

		}
		//printf("fnd digit 0.1=%d\n",display_value);
	}else if(digit==10){
		sprintf(source_buf,"%04.1f",output_value);
		//printf("source_buf=%s\n",source_buf);

		//  *1 ����
		memset(dest_buf,0,sizeof(dest_buf));
		strncpy(dest_buf, (source_buf+1), 1);
		display_value=atoi(dest_buf);
		FndShadowOutput(FND3_CLK ,FndTable[display_value] & FndTable[DOT]);
		//printf("fnd digit 10=%d\n",display_value);

		//  *10 ����
		memset(dest_buf,0,sizeof(dest_buf));
		strncpy(dest_buf, (source_buf+0), 1);
		display_value=atoi(dest_buf);
		FndShadowOutput(FND2_CLK ,(FndTable[display_value]));
		//printf("fnd digit 1=%d\n",display_value);

		// *100 ����
		FndShadowOutput(FND1_CLK ,FndTable[0]);
		//printf("fnd digit 0.1=%d\n",display_value);
	}else if(digit==100){
		sprintf(source_buf,"%04.1f",output_value);
		if(output_value <100.0){
			//  *1 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+1), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] & FndTable[DOT]);
			//printf("fnd digit 10=%d\n",display_value);

			//  *10 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,(FndTable[display_value]));
			//printf("fnd digit 1=%d\n",display_value);

			// *100 ����
			FndShadowOutput(FND1_CLK ,FndTable[0]);
		}else{
			//  *1 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+2), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND3_CLK ,FndTable[display_value] & FndTable[DOT]);
			//printf("fnd digit 10=%d\n",display_value);

			//  *10 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+1), 1);
			display_value = atoi(dest_buf);
			FndShadowOutput(FND2_CLK ,(FndTable[display_value]));
			//printf("fnd digit 1=%d\n",display_value);

			// *100 ����
			memset(dest_buf,0,sizeof(dest_buf));
			strncpy(dest_buf, (source_buf+0), 1);
			display_value=atoi(dest_buf);
			FndShadowOutput(FND1_CLK ,(FndTable[display_value]));
		}
	}

}

/**
  * @brief FndDisplayString
  * @param uint8_t *str
  * @retval	 None
  */
void FndDisplayString(uint8_t strValue)
{
	FndAllOff();
	switch (strValue)
	{
	case EDIT:
		FndShadowOutput(FND1_CLK ,0x84); //e
		FndShadowOutput(FND2_CLK ,0xa1); //d
		FndShadowOutput(FND3_CLK ,0x87); //t

		break;
	case RUN:
		FndShadowOutput(FND1_CLK ,0x88); //r
		FndShadowOutput(FND2_CLK ,0xc1); //u
		FndShadowOutput(FND3_CLK ,0xc8); //n

		break;

	default:
		break;

	}
}

/**
  * @brief FndDispalyParameter
  * @param uint8_t Parameter
  * @retval	 None
  */
void FndDispalyParameter(uint8_t Parameter){
	FndAllOff();
	switch(Parameter){
		case 1: //POS
			// *10 ����
			FndOutput(FND1_CLK ,0x8C); //P
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[1]);
			break;
		case 2 ://�˶�üũ ����
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[2]);
			break;
		case 3 ://P
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[3]);
			break;
		case 4 ://I
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[4]);
			break;
		case 5: //D
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[5]);
			break;
		case 6: //Max
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[6]);
			break;
		case 7: //Min
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[7]);
			break;
		case 8: //Control Mode
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[8]);
			break;
		case 9: //Display Mode
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[0]);
			FndOutput(FND3_CLK ,FndTable[9]);
			break;
		case 10: //Multi Tension
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[0]);
			break;
		case 11: //Load Cell
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[1]);
			break;
		case 12: //Gain
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[2]);
			break;
		case 13:
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[3]);
			break;
		case 14:
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[4]);
			break;
		case 15:
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[5]);
			break;
		case 16:
			FndOutput(FND1_CLK ,0x8C);
			FndOutput(FND2_CLK ,FndTable[1]);
			FndOutput(FND3_CLK ,FndTable[6]);
			break;
		default:
			break;
	}

}

void FndIncrementBar(uint8_t fndPos){
	if(fndPos == 1){
		FndOutput(FND3_CLK ,FndTable[BAR] & FndTable[DOT]);
	}else if(fndPos == 2){
		FndOutput(FND2_CLK ,FndTable[BAR] & FndTable[DOT]);
	}else if(fndPos == 3){
		FndOutput(FND1_CLK ,FndTable[BAR] & FndTable[DOT]);
	}
}

/**
  * @brief FndOutput
  * @param uint8_t fnd_pos ,uint8_t fnd_vlaue
  * @retval	 None
  */
void FndShadowOutput(uint8_t fndPos ,uint8_t fndVlaue)
{
	if(fndPos == FND1_CLK) ShadowFnd1=fndVlaue;
	else if(fndPos == FND2_CLK)ShadowFnd2=fndVlaue;
	else if(fndPos == FND3_CLK) ShadowFnd3=fndVlaue;
	else ShadowLed =fndVlaue;
	Delay_ms(2);
	GPIOD->ODR  = (uint32_t)fndVlaue;  	//Fnd Display Value
	Delay_ms(2);
	GPIOE->BSRR = (uint32_t)fndPos;		//Fnd Contrl Signal
	Delay_ms(2);
	GPIOE->BRR = (uint32_t)fndPos;
}

void FndOutput(uint8_t fndPos ,uint8_t fndVlaue)
{
	GPIOD->ODR  = (uint32_t)fndVlaue;  	//Fnd Display Value
	Delay_ms(2);
	GPIOE->BSRR = (uint32_t)fndPos;		//Fnd Contrl Signal
	Delay_ms(2);
	GPIOE->BRR = (uint32_t)fndPos;
}

void FndAllOff(void)
{
	FndOutput(FND1_CLK ,0xFF);
	FndOutput(FND2_CLK ,0xFF);
	FndOutput(FND3_CLK ,0xFF);
	ShadowFnd1=0xFF;
	ShadowFnd2=0xFF;
	ShadowFnd3=0xFF;
	FndDisplayShadow();
}

void FndAllOn(void)
{
	FndOutput(FND1_CLK ,0x00);
	FndOutput(FND2_CLK ,0x00);
	FndOutput(FND3_CLK ,0x00);
	ShadowFnd1=0x00;
	ShadowFnd2=0x00;
	ShadowFnd3=0x00;
	FndDisplayShadow();
}

void FndDisplayShadow(void)
{
	FndShadowOutput(FND1_CLK ,ShadowFnd1);
	FndShadowOutput(FND2_CLK ,ShadowFnd2);
	FndShadowOutput(FND3_CLK ,ShadowFnd3);
	FndShadowOutput(LED_CLK ,ShadowLed);
}
/**
  * @brief FndFlicker
  * @param uint8_t onOff
  * @retval	 None
  */
void FndFlicker(uint8_t onOff)
{
static uint8_t status=OFF;

	if(onOff == ON){
		if(status==ON){
			FndDisplayShadow();
			status=OFF;
		}else{
			FndAllOff();
			status=ON;
		}
	}else{
		status=ON;
		FndDisplayShadow();
	}
}


void PannelLEdFlicker(uint8_t ledPos,uint8_t onOff)
{
static uint16_t DownDelayCount=0,UpDelayCount=0,RunningDelayCount=0;
static uint8_t inverse1=OFF,inverse2=OFF,inverse3=OFF;

if(ledPos==DOWN)
{
	if (onOff == ON){
		if(DownDelayCount >10){
			if(inverse1 == ON){
				PannelLed(DOWN,OFF);  //low
				inverse1=OFF;
			}else{
				PannelLed(DOWN,ON);
				inverse1=ON;
			}
			DownDelayCount=0;
		}else{
			DownDelayCount++;
		}
	}else{
		DownDelayCount=0;
	}
}

if(ledPos==UP)
{
	if (onOff == ON){
		if(UpDelayCount >10){
			if(inverse2 == ON){
				PannelLed(UP,OFF);  //low
				inverse2=OFF;
			}else{
				PannelLed(UP,ON);
				inverse2=ON;
			}
			UpDelayCount=0;
		}else{
			UpDelayCount++;
		}
	}else{
		UpDelayCount=0;
	}
}

if(ledPos==RUNNING)
{
	if (onOff == ON){
		if(RunningDelayCount >10){
			if(inverse3 == ON){
				PannelLed(RUNNING,OFF);  //low
				inverse3=OFF;
			}else{
				PannelLed(RUNNING,ON);
				inverse3=ON;
			}
			RunningDelayCount=0;
		}else{
			RunningDelayCount++;
		}
	}else{
		RunningDelayCount=0;
	}
}


}
void PannelLEdAll(uint8_t onOff)
{
	if (onOff == ON){
		PannelLed(DOWN,ON);
		PannelLed(UP,ON);
		PannelLed(RUNNING,ON);
	}else{
		PannelLed(DOWN,OFF);
		PannelLed(UP,OFF);
		PannelLed(RUNNING,OFF);
	}
}
/**
  * @brief PannelLed
  * @param uint8_t ledPos,uint8_t onOff
  * @retval	 None
  */
void PannelLed(uint8_t ledPos,uint8_t onOff)
{
	if(onOff==ON){
		ShadowLed = ShadowLed & ~(1<<ledPos);
	}else{
		ShadowLed = ShadowLed | (1<<ledPos);
	}
	FndDisplayShadow();
}
/**
  * @brief GetButtonStatus
  * @param Nothing
  * @retval	 uint8_t
  */

uint8_t GetButtonStatus(void)
{
volatile uint8_t keyValue=0;
	keyValue=(uint8_t)GPIOC->IDR;
	return keyValue= ~(keyValue >> 2) & 0X07;
}

GPIO_PinState GetInput(uint16_t input)
{
	return HAL_GPIO_ReadPin(GPIOB,input);
}


void SetOutput(uint16_t output,uint8_t status)
{
	if(status==ON){
		HAL_GPIO_WritePin(GPIOE,output,GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOE,output,GPIO_PIN_RESET);
	}
}


/*
**********************************************************************************************************
//Define parameter
#define epsilon 0.01
#define dt 0.01             //100ms loop time
#define MAX  100
#define MIN -50			 	//�ټǹ� ����������
#define Kp  1.0
#define Kd  0.1	//0.1
#define Ki  0.9	//0.9
***********************************************************************************************************
*/
//The Matlab simulation of the PID algorithm to control the DC motor
float PIDcal(float setpoint,float actual_position)// ���簪
{
	static float pre_error = 0;		//���� ����
	static float integral = 0;  	//���� ����
	float error;					//���� ����
	float derivative;				//���� �̺�
	float output;					//���

	//Caculate P,I,D
	error = (setpoint - actual_position)  ;

	//In case of error too small then stop intergration
	if(pre_error != error){
		integral = integral + error * dt;
	}else{
		integral=0;
	}

	if(error == 0)integral=0;
	derivative = (error - pre_error)/dt;						// �����̺� = (�������-��������)/dt
	//output = (Kp*error) + (Kd*derivative) + (Ki* integral);		// ��� = p��+i��+d��
	output = (Kp*error) + (Kd*derivative);		// ��� = p��+i��+d��
	//Saturation Filter
	if(output > MAX){
		output = MAX;
	}
	else if(output < MIN){
		output = MIN ;
	}
    //Update error ��������� ����������
    pre_error = error ;

 return output;
}


void CheckWireAlram(uint8_t op){
	if(op ==ON){
		HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin,GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin,GPIO_PIN_RESET);
	}
}


















