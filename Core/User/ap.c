/*
 * app.c
 *
 *  Created on: Apr 12, 2025
 *      Author: cikkr
 */
#include "ap.h"


extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

/* Private variables ---------------------------------------------------------*/
uint16_t dacVal[2] = {0, 4095};
uint16_t adcVal[4];
uint8_t IsRotaryEncoderStatus=ENCODER_STOP;
uint8_t  IsFndFlicker=FLICKER_OFF;
uint8_t IsKeyStatus=KeyInitStatus, IsEditStatus=EDIT_OFF;
uint8_t IsRunMode;

////////////////////////////////////////////////////////////
#define ANALOG_TIMER_INTERVAL 1 //milliseconds

int32_t analogPinTimer = 0;

uint32_t currentTick, beforeOldTick, deltaTick;
int fc = 20; //cutoff frequency 5~10HZ
double dts= ANALOG_TIMER_INTERVAL/1000.0f;
double lambda =0.0;
double x = 0.0;
double x_f = 0.0;
double x_fold = 0.0;

float refVoltage;
////////////////////////////////////////////////////////////
volatile int8_t keyValue=0;
volatile uint16_t keyDelayCount=0,displayDelayCount=0,encoderCount=0;

volatile uint8_t lastEncoded = 0;
volatile uint32_t DacValue;

uint8_t OffsetCnt;
uint8_t Parameter = 1;

float ConvCalLodecell,ConvCalBreakVoltage;
char readBuf[10];

volatile float BarAngle,WireCheckAngle;
volatile float K_p,K_i,K_d,K_min,K_max;
volatile float StepTension,AdGain,fadcVal,CompfadcVal;
volatile float fAvrAdc_PotenionVal;
volatile uint8_t CurrentModel,DisplayMode,BackupModel;
volatile uint16_t LoadCell_OffSetValue=0;
volatile uint16_t LoadCell_SpanValue=0,LoadCell_SpanValueLow=0,LoadCell_SpanValueHigh=0;
volatile uint16_t MaxValue=0,MinValue=0;
volatile float OffsetB;


uint32_t averageAdValue0=0;
uint8_t icnt=0;
uint16_t max=0,min=0;
uint16_t ADarray[8],adMem[8];
int16_t EncoderButtonDelayCount=0;

float alpha = 0.1f;             // 필터 계수 0.01 ~ 0.1: 부드럽지만 반응 느림 (노이즈 제거에 강함)
float filtered = 0.0f;          // 초기 필터 출력값 반응 빠름, 노이즈도 함께 전달됨
float lpf(float input, float prev_output, float alpha);

void subTask(void *argument)
{
	volatile uint32_t averageAdValue1=0;
	volatile uint8_t averageCounter=0;
	float controloutput=0;
	float offsetVal=0;
	osDelay(2500);  //2.5sec
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adcVal,4);

	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)&dacVal[0], 1, DAC_ALIGN_12B_R);
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)&dacVal[1], 1, DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim6);
	HAL_TIM_Base_Start(&htim7);


	for(;;)
	{
		uint16_t compVal1;
		uint16_t compVal2;
		if (IsRunMode == RUN){
			if(GetInput(MULTI_TENSION2) == GPIO_PIN_SET){
				//FndDisplay(StepTension,1);
				//DacValue =(StepTension*4096)/24;
				//printf(" MULTI_TENSION2 K_max=%f\r\n",K_max);
				if(averageCounter<8){
					averageAdValue1 += adcVal[1]; //ad��ȯ
				}else{
					ConvCalBreakVoltage = averageAdValue1/8;//3.3/4095=0.000805
					averageAdValue1 = 0;
					averageCounter  = 0;
					refVoltage=ConvCalBreakVoltage *0.00854f;
					compVal1 = refVoltage*10;
					compVal2 = K_max*10;

					if(compVal1 > compVal2){
						if(compVal1 != (compVal2+1))offsetVal -= 0.1;
					}else if(compVal1 < compVal2){
						if(compVal1 != (compVal2-1))offsetVal += 0.1;
					}
					if(offsetVal>4)offsetVal=4;
					else DacValue = (uint32_t) ((K_max+offsetVal)/0.0072);
					dacVal[0] = DacValue;
				}
				averageCounter++;
			}else if(GetInput(MULTI_TENSION)== GPIO_PIN_SET){
				//FndDisplay(StepTension,1);
				//DacValue =(StepTension*4096)/24;
				//printf(" MULTI_TENSION StepTension=%f\r\n",StepTension);
				if(averageCounter<8)
				{
					averageAdValue1 += adcVal[1]; //ad��ȯ
				}else{
					ConvCalBreakVoltage = averageAdValue1/8;//3.3/4095=0.000805
					averageAdValue1 = 0;
					averageCounter  = 0;
					refVoltage=ConvCalBreakVoltage *0.00854f;
					compVal1 = refVoltage*10;
					compVal2 = StepTension*10;

					if(compVal1 > compVal2){
						if(compVal1 != (compVal2+1))offsetVal -= 0.1;
					}else if(compVal1 < compVal2){
						if(compVal1 != (compVal2-1))offsetVal += 0.1;
					}
					if(offsetVal>4)offsetVal=4;
					else DacValue = (uint32_t) ((StepTension+offsetVal)/0.0072);
					dacVal[0] = DacValue;
				}
				averageCounter++;
			}else{
				if(averageCounter<8){
					averageAdValue1 += adcVal[1]; //ad��ȯ
				}else{
					ConvCalBreakVoltage = averageAdValue1/8;//3.3/4095=0.000805
					averageAdValue1 = 0;
					averageCounter  = 0;
					refVoltage=ConvCalBreakVoltage *0.00854f;
					compVal1 = refVoltage*10;
					compVal2 = fadcVal*10;

					if(compVal1 > compVal2){
						if(compVal1 != (compVal2+1))offsetVal -= 0.1;
					}else if(compVal1 < compVal2){
						if(compVal1 != (compVal2-1))offsetVal += 0.1;
					}
					if(offsetVal>4)offsetVal=4;
					else DacValue = (uint32_t) ((fadcVal+offsetVal)/0.0072);
					dacVal[0] = DacValue;
				}
				averageCounter++;
			}
			fAvrAdc_PotenionVal = ((float)adcVal[3]/1024)*360;
		}
		osDelay(10);
	}

}


void mainTask(void *argument)
{
	int16_t scanCounter=0,resetButtonDelayCount=0,offsetCount=0;
	int8_t i;
	volatile float faverageAdValue0,OffSetValue,SpanPvalue,SpanEvalue;

	IsRunMode=RUN;
	osDelay(2500);  //2.5sec
	HAL_GPIO_WritePin(FND_nOE_GPIO_Port,FND_nOE_Pin, GPIO_PIN_RESET);
	MX_I2C1_Init();
	//ResetParameter(0);
	LoadParameter(0);
	for(;;)
	{
		PannelLEdFlicker(RUNNING,ON);
		keyValue=GetButtonStatus();
		//printf(" potension AD value =%ld\n",adcVal[3]);
		if ((keyValue == KEY_RUNEDIT) && ( IsRunMode==RUN) && ( IsKeyStatus==KeyInitStatus) ){
			//printf(" Mode is Edit\n");
			IsRunMode 		= EDIT;
			IsKeyStatus 	= KeyDownStatus;
			IsFndFlicker	=	FLICKER_OFF;
			FndDisplayString(EDIT);
			osDelay(500);
		}else if ((keyValue == KEY_RUNEDIT) && (IsRunMode==EDIT) && (IsKeyStatus==KeyInitStatus)){
			//printf(" Mode is Run\n");
			IsRunMode 	= RUN;
			IsKeyStatus = KeyDownStatus;
			FndDisplayString(RUN);
			osDelay(500);
		}else if (keyValue == ENCODER_BUTTON){
			FndAllOff();
			EncoderButtonDelayCount++;
			if(EncoderButtonDelayCount>10){
				osDelay(1000);
				IsRunMode=CAL_ADJUST;
				//printf(" Mode is cal_Adjust\n");

				averageAdValue0 =0;
				osDelay(200);
				FndIncrementBar(1);
				memset(ADarray,0,sizeof(ADarray));
				for(i=0;i<ArraySize;i++){
					ADarray[i]=adcVal[0]; //ad��ȯ
					osDelay(10);
				}
				max=ADarray[0];
				min=ADarray[0];
				icnt=0;
				for(i=0;i<ArraySize;i++){
					if(ADarray[i]>max){
						max=ADarray[i];
					}else if(ADarray[i]<min){
						min=ADarray[i];
					}else{
						averageAdValue0+=ADarray[i];
						icnt++;
					}
				}

				averageAdValue0=averageAdValue0 /icnt;
				LoadCell_OffSetValue=(uint16_t)averageAdValue0;

				osDelay(200);
				FndIncrementBar(2);
				//printf(" LoadCell_OffSetValue=%ld\n",(unsigned long)averageAdValue0);
				DataManager(CmdWriteEeprom,CurrentModel,PAR_LOADCELL_OFFSET);

				EncoderButtonDelayCount=0;
				osDelay(200);
				FndIncrementBar(3);
				IsRunMode=RUN;
			}
		}else if (keyValue == KEY_RESET){
			printf(" Key is Reset\n");
			resetButtonDelayCount++;
			if(resetButtonDelayCount>20){
				//				printf(" Key is Reset Entry\n");
				IsFndFlicker=FLICKER_ON;
				ResetParameter(CurrentModel);
				LoadParameter(CurrentModel);
				resetButtonDelayCount=0;
				osDelay(500);
				IsFndFlicker=FLICKER_OFF;
			}
		}else{
			resetButtonDelayCount=0;
			EncoderButtonDelayCount=0;
		}


		/* Flicker On/Off  */
		if(IsFndFlicker == FLICKER_ON){
			FndFlicker(ON);
		}else{
			FndFlicker(OFF);
		}


		/////////////////////////////////////////////////////////////////////////////////////////////
		if (IsRunMode == RUN){
			SpanPvalue=(500.0f-100.0f)/(float)(LoadCell_SpanValueHigh-LoadCell_SpanValueLow);  //��ʰ�
			SpanEvalue=100-(LoadCell_SpanValueLow* SpanPvalue);// 100g�� ���� ����

			currentTick= HAL_GetTick();
			if(beforeOldTick != currentTick){
				deltaTick=currentTick - beforeOldTick;
				beforeOldTick = currentTick;
			}
			/*
			filtered  = lpf((float)adcVal[0],filtered, alpha);

			faverageAdValue0=(filtered *SpanPvalue)+SpanEvalue;
			OffSetValue=(LoadCell_OffSetValue*SpanPvalue)+SpanEvalue;
			if((faverageAdValue0-OffSetValue)>=0){
				faverageAdValue0=faverageAdValue0-OffSetValue;
			}else{
				faverageAdValue0=0;
			}*/

			analogPinTimer -= deltaTick;
			//printf("beforeOldTick=%ld, beforeOldTick=%ld ,deltaTick=%ld\r\n",beforeOldTick,currentTick,deltaTick);
			//printf("analogPinTimer=%ld \r\n",analogPinTimer); LoadCell_OffSetValue
			if (analogPinTimer <= 0){
				analogPinTimer += ANALOG_TIMER_INTERVAL;
				// sensing loop start!!
				x = adcVal[0]; // �Ƴ��αװ� �б�  LoadCell_OffSetValue
				lambda = 2*PI*fc*dts;
				x_f = (lambda/(1+lambda)*x )+(1/(1+lambda)*x_fold); //���͵� ��
				x_fold = x_f; // ���� ���� ������ ������Ʈ
				faverageAdValue0=floor(x_f + 0.5 );//
				faverageAdValue0=(faverageAdValue0*SpanPvalue)+SpanEvalue;

				OffSetValue=(LoadCell_OffSetValue*SpanPvalue)+SpanEvalue;
				if((faverageAdValue0-OffSetValue)>=0){
					faverageAdValue0=faverageAdValue0-OffSetValue;
				}else{
					faverageAdValue0=0;
				}
			}

			///////////////////////////////////////////////////////////////////////////////////


			if(DisplayMode == SET_VALUE ){ // ��Ʈ ������
				if (keyValue== KEY_SET){
					FndDisplay (fadcVal*K_p,1); // ������а� ǥ��
				}else{
					if(faverageAdValue0 > 99.9) FndDisplay(faverageAdValue0*AdGain,100);
					else FndDisplay(faverageAdValue0*AdGain,10);
				}
			}else{//REAL_VALUE
				if (keyValue == KEY_SET){
					if(faverageAdValue0 >99.9) FndDisplay((faverageAdValue0*AdGain),100);
					else FndDisplay((faverageAdValue0* AdGain),10);
				}else{
					if(GetInput(MULTI_TENSION)== GPIO_PIN_SET){
						FndDisplay ((StepTension*K_p),1);
					}
					else if(GetInput(MULTI_TENSION2)== GPIO_PIN_SET){
						FndDisplay (K_max,1);
					}
					else
					{
						FndDisplay ((fadcVal*K_p),1);
					}
				}
			}

			if(faverageAdValue0<MinValue){ //300
				PannelLed(DOWN,ON);
				PannelLed(UP,OFF);
				SetOutput(WIRE_STATUS,ON);

			}else if(faverageAdValue0>MaxValue){ //100
				PannelLed(DOWN,OFF);
				PannelLed(UP,ON);
				SetOutput(WIRE_STATUS,ON);
			}else{
				PannelLed(DOWN,OFF);
				PannelLed(UP,OFF);
				SetOutput(WIRE_STATUS,OFF);
			}

			if(fAvrAdc_PotenionVal <= WireCheckAngle){
				CheckWireAlram(OFF);
				//printf("on Wire \n");
			}else{

				CheckWireAlram(ON);
				//printf("off Wire \n");
			}
			///////////////////////////////////////////////////////////////////////////////////

		}else if(IsRunMode == EDIT){
			FndDispalyParameter(Parameter);
			while(1){
				if(keyValue == 0)IsKeyStatus = KeyInitStatus;
				keyValue=GetButtonStatus();
				keyDelayCount++;
				displayDelayCount++;
				if(displayDelayCount >10){
					displayDelayCount=0;
					FndDispalyParameter(Parameter);
				}

				if(CurrentModel !=  BackupModel){
					BackupModel = CurrentModel;
					DataManager(CmdWriteEeprom,0,PAR_BackupModel);
					osDelay(500);
					FndIncrementBar(1);
					osDelay(500);
					FndIncrementBar(2);
					osDelay(500);
					FndIncrementBar(3);
					LoadParameter(BackupModel);
				}

				if(keyDelayCount >10){
					keyDelayCount=0;
					if ((keyValue == KEY_SET) && (IsKeyStatus==KeyInitStatus)){
						IsRunMode = EDIT_DATA;
						IsKeyStatus=KeyDownStatus;
						IsEditStatus=EDIT_ON;
						printf(" Key is Entry\n");
						osDelay(1000);
						encoderCount=0;
						while(1){
							ParamterSet(Parameter);
							if(IsEditStatus == EDIT_OFF)break; //while break;
						}
					}
				}else if (keyValue == KEY_RUNEDIT){
					printf(" Mode is Run\n");
					IsKeyStatus = KeyInitStatus;
					IsRunMode=RUN;
					osDelay(500);
					break;
				}

			}
		}
		osDelay(1);
		//printf("===========%f \r\n",ConValue);
		//printf("dacVal[0] =%d  ,",dacVal[0]);
		//printf("dacVal[1] =%d  \r\n",dacVal[1]);
		//printf("adcVal[0] =%ld  ,",adcVal[0]);
		//printf("adcVal[1] =%ld  ,",adcVal[1]);
		//printf("adcVal[2] =%ld  ,",adcVal[2]);
		//printf("adcVal[3] =%ld \r\n",adcVal[3]);
	}
}

float lpf(float input, float prev_output, float alpha) {
    return alpha * input + (1.0f - alpha) * prev_output;
}


void UpDateEncoder(void)
{
	uint8_t MSB = (uint8_t) HAL_GPIO_ReadPin(ENC_A_GPIO_Port,ENC_A_Pin);	//MSB = most significant bit
	uint8_t LSB = (uint8_t) HAL_GPIO_ReadPin(ENC_B_GPIO_Port,ENC_B_Pin);	//LSB = least significant bit
	uint8_t encoded = (MSB << 1) | LSB;								//converting the 2 pin value to single number
	uint8_t sum  = (lastEncoded << 2) | encoded;					//adding it to the previous encoded value
	//printf("UpDateEncoder=%x\n",sum);

	if(sum ==0x0d || sum ==0x04  || sum ==0x02  || sum ==0x0b ){
		//printf("Rotary sw CW  ====>  \r\n");
		OffsetCnt++;
		if(OffsetCnt>3){
			OffsetCnt=0;
			if(IsRunMode == EDIT){
				Parameter++;
				if(Parameter < 1) Parameter=16;
				else if (Parameter >16)Parameter=1;
			}else if(IsRunMode == EDIT_DATA){
				switch(Parameter){
				case PAR_POS: //1
					break;
				case PAR_CHK_ANG:	//2
					break;
				case PAR_P://3
					if(K_p>9.9)K_p=K_p+0.1;
					else K_p=K_p+0.01;
					if(K_p >99.9)K_p=0;
					break;
				case PAR_I:
					if(K_i>9.9)K_i=K_i+0.1;
					else K_i=K_i+0.01;
					if(K_i >99.9)K_i=99.9;
					break;
				case PAR_D:
					if(K_d>9.9)K_d=K_d+0.1;
					else K_d=K_d+0.01;
					if(K_d >99.9)K_d=99.9;
					break;
				case PAR_MAX:
					K_max=K_max+0.1;
					if(K_max>24.0) K_max=24.0;
					break;
				case PAR_MIN:
					K_min=K_min+0.1;
					if(K_min > 24.0) K_min=24.0;
					break;
				case PAR_MODEL://Control mode
					if(CurrentModel < 3){
						if(CurrentModel >= 0){
							CurrentModel = CurrentModel+1;
							if(CurrentModel > 2) CurrentModel=2;
						}else{
							CurrentModel = 0;
						}
					}else{
						CurrentModel = 2;
					}
					break;
				case PAR_DISP://Display Mode
					if(DisplayMode < 3) DisplayMode=DisplayMode+1;
					else DisplayMode=2;

					break;
				case PAR_MULTI://Multi Tension
					if(StepTension < 24){
						if(StepTension >= 0.0){
							StepTension = StepTension+0.1;
						}else{
							StepTension = 0;
						}
					}else{
						StepTension = 24;
					}
					break;
				case PAR_LOADCELL_OFFSET://
					if(LoadCell_OffSetValue < 100){
						if(LoadCell_OffSetValue >= 0){
							LoadCell_OffSetValue = LoadCell_OffSetValue+1;
						}else{
							LoadCell_OffSetValue = 0;
						}
					}else{
						LoadCell_OffSetValue = 100;
					}
					break;
				case PAR_ADGAIN://
					if(AdGain < 10){
						if(AdGain>= 0.0){
							AdGain=AdGain+0.1;
						}else{
							AdGain=0;
						}
					}else{
						AdGain=10;
					}
					break;
				case PAR_PLIMIT://
					if(MaxValue < 999){
						if(MaxValue>= 0){
							MaxValue=MaxValue+1;
						}else{
							MaxValue=0;
						}
					}else{
						MaxValue=999;
					}
					break;
				case PAR_MLIMIT://
					if(MinValue < 999){
						if(MinValue>= 0){
							MinValue=MinValue+1;
						}else{
							MinValue=0;
						}
					}else{
						MinValue=999;
					}
					break;

				case PAR_LOADCELL_SPAN_L://
					if(LoadCell_SpanValueLow < 1500){
						if(LoadCell_SpanValueLow> 0){
							LoadCell_SpanValueLow=LoadCell_SpanValueLow+1;
						}else{
							LoadCell_SpanValueLow=0;
						}
					}else{
						LoadCell_SpanValueLow=1500;
					}
					break;
				case PAR_LOADCELL_SPAN_H://
					if(LoadCell_SpanValueHigh < 1500){
						if(LoadCell_SpanValueHigh> 0){
							LoadCell_SpanValueHigh=LoadCell_SpanValueHigh+1;
						}else{
							LoadCell_SpanValueHigh=0;
						}
					}else{
						LoadCell_SpanValueHigh=1500;
					}
					break;

				default:
					//printf(" Display default\n");
					break;
				}//end of switch
			}else{  //Run

				fadcVal=fadcVal+0.1;
				if(fadcVal < 0) fadcVal=0;
				else if (fadcVal >24)fadcVal=24;
				DataManager(CmdReadVariable,CurrentModel,PAR_fAdcValue);//CompfadcVal�� ���� ����
				if(CompfadcVal != fadcVal){
					DataManager(CmdWriteEeprom,CurrentModel,PAR_fAdcValue);////�ټǺ������� ����
				}
				//DacValue=(uint32_t) (fadcVal/0.008);
				//WrEepromFloat(fadcVal,100+(150*CurrentModel));	//�ټǺ���
			}
		}
	}
	if(sum == 0x0e || sum == 0x07 || sum == 0x01 || sum == 0x08){
		//printf("<========Rotary sw CCW  \r\n");
		OffsetCnt++;
		if(OffsetCnt>3){
			OffsetCnt=0;
			if(IsRunMode == EDIT){
				Parameter--;
				if(Parameter < 1) Parameter=16;
				else if (Parameter >16)Parameter=1;
			}else if(IsRunMode == EDIT_DATA){

				switch(Parameter){
					case PAR_POS: //1
							break;
					case PAR_CHK_ANG:	//2
							break;
					case PAR_P://3
							if(K_p>9.9)K_p=K_p-0.1;
							else K_p=K_p-0.01;
							if(K_p <0)K_p=0.0;
							break;
					case PAR_I:
							if(K_i>9.9)K_i=K_i-0.1;
							else K_i=K_i-0.01;
							if(K_i <0)K_i=0;
							break;
					case PAR_D:
							if(K_d>9.9)K_d=K_d-0.1;
							else K_d=K_d-0.01;
							if(K_d <0.0)K_d=0;
							break;
					case PAR_MAX:
							K_max=K_max-0.1;
							if(K_max < 0.0) K_max=0.0;
							break;
					case PAR_MIN:
							K_min=K_min-0.1;
							if(K_min <0.0) K_min=0.0;

							break;
					case PAR_MODEL://Control mode
							if(CurrentModel < 3){
								if(CurrentModel > 0){
									CurrentModel = CurrentModel-1;
									if(CurrentModel < 0) CurrentModel=0;
								}else{
									CurrentModel = 0;
								}
							}else{
								CurrentModel = 2;
							}
							break;
					case PAR_DISP://Display Mode
							if(DisplayMode > 0) DisplayMode=DisplayMode-1;
							else DisplayMode=2;
							break;
					case PAR_MULTI://Multi Tension
							if(StepTension <=24.0) {
								if(StepTension >0.0){
									StepTension = StepTension-0.1;
								}else{
									StepTension=0;
								}
							}else{
								StepTension = 24;
							}
							break;
					case PAR_LOADCELL_OFFSET://
							if(LoadCell_OffSetValue <=100){
								if(LoadCell_OffSetValue> 0){
									LoadCell_OffSetValue=LoadCell_OffSetValue-1;
								}else{
									LoadCell_OffSetValue=0;
								}
							}else{
								LoadCell_OffSetValue=100;
							}
							break;
					case PAR_ADGAIN://
							if(AdGain <= 10){
								if(AdGain> 0.0){
									AdGain = AdGain-0.1;
								}else{
									AdGain = 0;
								}
							}else{
								AdGain=10;
							}
							break;
					case PAR_PLIMIT://
							if(MaxValue <= 999){
								if(MaxValue> 0){
									MaxValue=MaxValue-1;
								}else{
									MaxValue=0;
								}
							}else{
								MaxValue=999;
							}
							break;
					case PAR_MLIMIT://
							if(MinValue <= 999){
								if(MinValue> 0){
									MinValue=MinValue-1;
								}else{
									MinValue=0;
								}
							}else{
								MinValue=999;
							}
							break;
					case PAR_LOADCELL_SPAN_L://
							if(LoadCell_SpanValueLow < 1500){
								if(LoadCell_SpanValueLow> 0){
									LoadCell_SpanValueLow=LoadCell_SpanValueLow-1;
								}else{
									LoadCell_SpanValueLow=0;
								}
							}else{
								LoadCell_SpanValueLow=0;
							}
							break;
					case PAR_LOADCELL_SPAN_H://
							if(LoadCell_SpanValueHigh < 1500){
								if(LoadCell_SpanValueHigh> 0){
									LoadCell_SpanValueHigh=LoadCell_SpanValueHigh-1;
								}else{
									LoadCell_SpanValueHigh=0;
								}
							}else{
								LoadCell_SpanValueHigh=0;
							}
							break;
					default:
							printf(" Display default\n");
						break;
				}//end of switch
			}else{

				fadcVal=fadcVal-0.1;
				if(fadcVal < 0) fadcVal=0;
				else if (fadcVal >24)fadcVal=24.0;

				DataManager(CmdReadVariable,CurrentModel,PAR_fAdcValue);//CompfadcVal�� ���� ����
				if(CompfadcVal != fadcVal){
					DataManager(CmdWriteEeprom,CurrentModel,PAR_fAdcValue);////�ټǺ������� ����
				}

				//DacValue=(uint32_t) (fadcVal/0.008);
				//WrEepromFloat(fadcVal,100+(150*CurrentModel));
			}
		}

	}
	lastEncoded = encoded;
}




void DataManager(uint8_t WhatCmd,uint8_t modelNumber,uint8_t parameter)
{
uint16_t memAddress=0,offsetNumber=40;
uint16_t GetValue,SetValue;
	memAddress= offsetNumber *  modelNumber;
	switch(parameter){
		case PAR_MODEL: //1
			//printf(" Display PAR_MODEL\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)CurrentModel;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+0,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+0, 1);
				CurrentModel=(uint8_t)GetValue;
				FndDisplay(CurrentModel,10);
			}else{//CmdReadVariable
				FndDisplay(CurrentModel,10);
			}
			break;
		case PAR_CHK_ANG://2
			//printf(" Display PAR_CHK_ANG\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(WireCheckAngle * 10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+1,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+1, 1);
				WireCheckAngle=(float)GetValue/10;
				FndDisplay(WireCheckAngle,0);
			}else{//CmdReadVariable
				FndDisplay(WireCheckAngle,0);
			}
			break;
		case PAR_P://3
			//printf(" Display PAR_P\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(K_p * 100);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+2,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+2, 1);
				K_p=(float)GetValue/100;
				FndDisplay(K_p,2);
			}else{//CmdReadVariable
				FndDisplay(K_p,2);
			}
			break;
		case PAR_I://4
			//printf(" Display PAR_I\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(K_i * 100);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+3,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+3, 1);
				K_i=(float)GetValue/100;
				FndDisplay(K_i,2);
			}else{//CmdReadVariable
				FndDisplay(K_i,2);
			}
			break;
		case PAR_D://5
			//printf(" Display PAR_D\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(K_d * 100);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+4,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+4, 1);
				K_d=(float)GetValue/100;
				FndDisplay(K_d,2);
			}else{//CmdReadVariable
				FndDisplay(K_d,2);
			}
			break;
		case PAR_MAX://6
			//printf(" Display PAR_MAX\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(K_max * 10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+5,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+5, 1);
				K_max=(float)GetValue/10;
				FndDisplay(K_max,1);
			}else{//CmdReadVariable
				FndDisplay(K_max,1);
			}
			break;
		case PAR_MIN://7
			//printf(" Display PAR_MIN\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(K_min * 10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+6,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+6, 1);
				K_min=(float)GetValue/10;
				FndDisplay(K_min,1);
			}else{//CmdReadVariable
				FndDisplay(K_min,1);
			}
			break;
		case PAR_POS: //8 Bar Postion
			//printf(" Display PAR_POS\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(BarAngle * 10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+7,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+7, 1);
				BarAngle=(float)GetValue/10;
				FndDisplay(BarAngle,0);
			}else{//CmdReadVariable
				FndDisplay(BarAngle,0);
			}
			break;
		case PAR_DISP://9 0 tension, 1 volt tension
			//printf(" Display PAR_DISP\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)DisplayMode;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+8,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+8, 1);
				DisplayMode=(uint8_t)GetValue;
				FndDisplay(DisplayMode,10);
			}else{
				FndDisplay(DisplayMode,10);
			}
			break;
		case PAR_MULTI://10
			//printf(" Display PAR_MULTI\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(StepTension * 10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+9,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+9, 1);
				StepTension=(float)GetValue/10;
				FndDisplay(StepTension,1);
			}else{//CmdReadVariable
				FndDisplay(StepTension,1);
			}
			break;
		case PAR_LOADCELL_OFFSET://11
			//printf(" Display PAR_LOADCELL_OFFSET\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)LoadCell_OffSetValue ;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+10,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+10, 1);
				LoadCell_OffSetValue=GetValue;
				FndDisplay(LoadCell_OffSetValue,100);
			}else{//CmdReadVariable
				FndDisplay(LoadCell_OffSetValue,100);
			}
			break;
		case PAR_ADGAIN://12
			//printf(" Display PAR_ADGAIN\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(AdGain *10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+11,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+11, 1);
				AdGain=(float)GetValue/10;
				FndDisplay(AdGain,1);
			}else{//CmdReadVariable
				FndDisplay(AdGain,1);
			}
			break;
		case PAR_PLIMIT://13
			//printf(" Display PAR_PLIMIT\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)MaxValue;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+12,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+12, 1);
				MaxValue=GetValue;
				FndDisplay(MaxValue,100);
			}else{//CmdReadVariable
				FndDisplay(MaxValue,100);
			}
			break;
		case PAR_MLIMIT://14
			//printf(" Display PAR_MLIMIT\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)MinValue;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+13,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+13, 1);
				MinValue=GetValue;
				FndDisplay(MinValue,100);
			}else{//CmdReadVariable
				FndDisplay(MinValue,100);
			}
			break;

		case PAR_LOADCELL_SPAN_L://15
			//printf(" Display PAR_LOADCELL_SPAN\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=LoadCell_SpanValueLow ;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+14,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+14, 1);
				LoadCell_SpanValueLow=GetValue;
				FndDisplay(LoadCell_SpanValueLow,100);
			}else{//CmdReadVariable
				FndDisplay(LoadCell_SpanValueLow,100);
			}
			break;
		case PAR_LOADCELL_SPAN_H://16
			//printf(" Display PAR_OFFSETB\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=LoadCell_SpanValueHigh ;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+15,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+15, 1);
				LoadCell_SpanValueHigh=GetValue;
				FndDisplay(LoadCell_SpanValueHigh,100);
			}else{//CmdReadVariable
				FndDisplay(LoadCell_SpanValueHigh,100);
			}
			break;
		case PAR_BackupModel://17
			//printf(" Display PAR_BackupModel\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)BackupModel;
				at24_HAL_WriteWords(&hi2c1, &SetValue ,16,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, 16, 1);
				BackupModel=(uint8_t)GetValue;
				//FndDisplay(BackupModel,10);
			}else{//CmdReadVariable
				FndDisplay(BackupModel,10);
			}
			break;
		case PAR_fAdcValue://18
			//printf(" Display PAR_fAdcValue\n");
			if(WhatCmd == CmdWriteEeprom){
				SetValue=(uint16_t)(fadcVal *10);
				at24_HAL_WriteWords(&hi2c1, &SetValue ,memAddress+17,1);
			}else if(WhatCmd == CmdReadEeprom){
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+17, 1);
				fadcVal=(float)GetValue/10;
				//FndDisplay(fadcVal,1);
			}else{//CmdReadVariable
				at24_HAL_ReadWords(&hi2c1, &GetValue, memAddress+17, 1);
				CompfadcVal=(float)(GetValue/10);
				//FndDisplay(fadcVal,1);
			}
			break;
		default:
			break;
	}
}



void LoadParameter(uint8_t modelNumber)
{
	uint16_t memAddress=0,offsetNumber=40;
	uint16_t Readbuf[30];

	memset(Readbuf,0,sizeof(Readbuf));
	memAddress= offsetNumber *  modelNumber;

	at24_HAL_ReadWords(&hi2c1, Readbuf, memAddress, 20);
	CurrentModel=(uint8_t)Readbuf[0];
	WireCheckAngle=(float)Readbuf[1]/10;
	K_p 	= (float)Readbuf[2]/10;
	K_i 	= (float)Readbuf[3]/10;
	K_d 	= (float)Readbuf[4]/100;
	K_max = (float)Readbuf[5]/10;
	K_min = (float)Readbuf[6]/10;
	BarAngle		= (float)Readbuf[7]/10;
	DisplayMode	= (uint8_t)Readbuf[8];
	StepTension	= (float)Readbuf[9]/10;
	LoadCell_OffSetValue = Readbuf[10];
	AdGain = (float)Readbuf[11]/10;
	MaxValue = Readbuf[12];
	MinValue = Readbuf[13];
	LoadCell_SpanValueLow = Readbuf[14];
	LoadCell_SpanValueHigh = Readbuf[15];
	BackupModel = (uint8_t)Readbuf[16];
	fadcVal = (float)Readbuf[17]/10;
	CurrentModel = modelNumber;
	BackupModel = modelNumber;

	/*
	printf(" =================\r\n");
	printf("0 CurrentModel =%d\n",CurrentModel);
	printf("1 WireCheckAngle =%f\n",WireCheckAngle);
	printf("2 K_p =%f\n",K_p);
	printf("3 K_i =%f\n",K_i);
	printf("4 K_d =%f\n",K_d);
	printf("5 K_max =%f\n",K_max);
	printf("6 K_min =%f\n",K_min);
	printf("7 BarAngle =%f\n",BarAngle);
	printf("8 DisplayMode =%d\n",DisplayMode);
	printf("9 StepTension =%f\n",StepTension);
	printf("10 LoadCell_OffSetValue =%d\n",LoadCell_OffSetValue);
	printf("11 AdGain =%f\n",AdGain);
	printf("12 MaxValue =%d\n",MaxValue);
	printf("13 MinValue =%d\n",MinValue);
	printf("14 LoadCell_SpanValueLow =%d\n",LoadCell_SpanValueLow);
	printf("15 LoadCell_SpanValueHigh =%d\n",LoadCell_SpanValueHigh);
	printf("16 BackupModel =%d\n",BackupModel);
	printf("17 fadcVal =%f\n",fadcVal);
	 */
}


/* ParamterSet function */
void ResetParameter(uint8_t modelNum)
{
	uint16_t memAddress=0,offsetNumber=40;
	uint16_t writebuf[30];

	memset(writebuf,0,sizeof(writebuf));
	memAddress= offsetNumber *  modelNum;
	writebuf[0]=(uint16_t)modelNum;		//current mode
	writebuf[1]=(uint16_t)(330.7*10);	//�ܼ���ġ
	writebuf[2]=(uint16_t)(0.9*10) ; 	//k_p
	writebuf[3]=(uint16_t)(0.4*10) ;	//k_i
	writebuf[4]=(uint16_t)(0.01*100);	//k_d
	writebuf[5]=(uint16_t)(99.9*10);	//max
	writebuf[6]=(uint16_t)(50.4*10);	//min
	writebuf[7]=(uint16_t)(300.5*10);  	//Bar Angle
	writebuf[8]=(uint16_t)0;			//display mode
	writebuf[9]=(uint16_t)(1.1*10);		//StepTension
	writebuf[10]=(uint16_t)(100);		//LoadCell_OffSetValue
	writebuf[11]=(uint16_t)(1.2*10);	//Gain
	writebuf[12]=(uint16_t)(300);		//���� MaxValue
	writebuf[13]=(uint16_t)(100);		//���� MinValue
	writebuf[14]=(uint16_t)(851);		//LoadCell_SpanValue
	writebuf[15]=(uint16_t)(100);	//OffsetB
	writebuf[16]=(uint16_t)modelNum;		//backup model
	writebuf[17]=(uint16_t)(12.0*10);	//��� ����
	at24_HAL_WriteWords(&hi2c1, writebuf ,memAddress,18);
}

/* ParamterSet function */
int8_t ParamterSet(uint8_t prmt)
{
	volatile int8_t fndFlickerOnCount=0,fndFlickerStatus=OFF;
	int16_t keyDelayCnt=0;
	uint32_t spanAverageAdValue=0;
	uint32_t PotenionAdValue=0;
	uint8_t i=0;
	DataManager(CmdReadEeprom,CurrentModel,prmt);
	IsRotaryEncoderStatus=ENCODER_STOP;
	while(1){
		keyValue=GetButtonStatus();
		if(IsRotaryEncoderStatus==ENCODER_RUN){
			encoderCount++;
		}

		if((encoderCount >EncoderTimeOut) && (IsRotaryEncoderStatus==ENCODER_RUN)){
			IsRotaryEncoderStatus=ENCODER_STOP;
			encoderCount=0;
		}

		if(IsRotaryEncoderStatus == ENCODER_STOP){
			if((fndFlickerStatus == OFF) && (fndFlickerOnCount >FlickerTimeOff)){
				FndAllOff();
				fndFlickerOnCount = 0;
				fndFlickerStatus = ON;
			}else if((fndFlickerStatus == ON) && (fndFlickerOnCount > FlickerTimeOn)){
				DataManager(CmdReadVariable,CurrentModel,prmt);
				fndFlickerOnCount = 0;
				fndFlickerStatus = OFF;
			}
			fndFlickerOnCount++;
		}else{
			DataManager(CmdReadVariable,CurrentModel,prmt);
			fndFlickerOnCount = 0;
			fndFlickerStatus = OFF;
		}
		//////////////////////////////////////////////////////////////////
		if ((keyValue == ENCODER_BUTTON)  && (prmt == PAR_CHK_ANG)){
			//printf(" PAR_CHK_ANG\n");
			while(1){
				FndAllOff();
				EncoderButtonDelayCount++;
				if(EncoderButtonDelayCount>10){
					osDelay(1000);
					FndIncrementBar(1);
					memset(ADarray,0,sizeof(ADarray));
					for(i=0;i<ArraySize;i++){
						ADarray[i] = adcVal[3]; //ad��ȯ
						osDelay(10);
					}
					max = adcVal[3];
					min = adcVal[3];
					icnt=0;
					for(i=0;i<ArraySize;i++){
						if(ADarray[i]>max){
							max = ADarray[i];
						}else if(ADarray[i]<min){
							min = ADarray[i];
						}else{
							PotenionAdValue += ADarray[i];
							icnt++;
						}
					}
					PotenionAdValue = PotenionAdValue /icnt;
					break;
				}
				osDelay(200);
			}
			osDelay(200);
			FndIncrementBar(2);
			WireCheckAngle=((float)PotenionAdValue/1024)*360;
			osDelay(200);
			FndIncrementBar(3);
			FndDispalyParameter(prmt);
			DataManager(CmdWriteEeprom,CurrentModel,prmt);
			IsEditStatus=EDIT_OFF;
			IsRunMode = EDIT;
		}
		//////////////////////////////////////////////////////////////////
		if ((keyValue == ENCODER_BUTTON)  && (prmt == PAR_LOADCELL_SPAN_L)){ //100g
			//printf(" loadcell span\n");
			while(1){
				FndAllOff();
				EncoderButtonDelayCount++;
				if(EncoderButtonDelayCount>10){
					osDelay(1000);
					spanAverageAdValue =0;
					osDelay(200);
					FndIncrementBar(1);
					memset(ADarray,0,sizeof(ADarray));
					for(i=0;i<ArraySize;i++){
						ADarray[i]=adcVal[0]; //ad��ȯ
						osDelay(10);
					}
					max=adcVal[0];
					min=adcVal[0];
					icnt=0;
					for(i=0;i<ArraySize;i++){
						if(ADarray[i]>max){
							max=ADarray[i];
						}else if(ADarray[i]<min){
							min=ADarray[i];
						}else{
							spanAverageAdValue+=ADarray[i];
							icnt++;
						}
					}
					spanAverageAdValue=spanAverageAdValue /icnt;
					break;
				}
				osDelay(200);
			}

			osDelay(200);
			FndIncrementBar(2);

			LoadCell_SpanValueLow=(uint16_t)(spanAverageAdValue);  //2���� �� ���ִϱ� �������̴�.
			//printf(" LoadCell_span=%lu\n",(unsigned long)((uint16_t)spanAverageAdValue - LoadCell_OffSetValue));

			osDelay(200);
			FndIncrementBar(3);
			FndDispalyParameter(prmt);
			DataManager(CmdWriteEeprom,CurrentModel,prmt);
			IsEditStatus=EDIT_OFF;
			IsRunMode = EDIT;
		}
		//////////////////////////////////////////////////////////////////
		if ((keyValue == ENCODER_BUTTON)  && (prmt == PAR_LOADCELL_SPAN_H)){  //500g
			//printf(" loadcell span\n");
			while(1){
				FndAllOff();
				EncoderButtonDelayCount++;
				if(EncoderButtonDelayCount>10){
					osDelay(1000);
					spanAverageAdValue =0;
					osDelay(200);
					FndIncrementBar(1);
					memset(ADarray,0,sizeof(ADarray));
					for(i=0;i<ArraySize;i++){
						ADarray[i]=adcVal[0]; //ad��ȯ
						osDelay(10);
					}
					max=adcVal[0];
					min=adcVal[0];
					icnt=0;
					for(i=0;i<ArraySize;i++){
						if(ADarray[i]>max){
							max=ADarray[i];
						}else if(ADarray[i]<min){
							min=ADarray[i];
						}else{
							spanAverageAdValue+=ADarray[i];
							icnt++;
						}
					}
					spanAverageAdValue=spanAverageAdValue /icnt;
					break;
				}
				osDelay(200);
			}

			osDelay(200);
			FndIncrementBar(2);

			LoadCell_SpanValueHigh=(uint16_t)(spanAverageAdValue);  //2���� �� ���ִϱ� �������̴�.
			//printf(" LoadCell_span=%lu\n",(unsigned long)((uint16_t)spanAverageAdValue - LoadCell_OffSetValue));

			osDelay(200);
			FndIncrementBar(3);
			FndDispalyParameter(prmt);
			DataManager(CmdWriteEeprom,CurrentModel,prmt);
			IsEditStatus=EDIT_OFF;
			IsRunMode = EDIT;
		}

		if(keyValue == KEY_SET) {
			keyDelayCnt++;
			if(keyDelayCnt>SetSaveCnt){
				//printf(" Parameter%d Saved\n",prmt);
				FndAllOff();
				osDelay(500);
				FndDispalyParameter(prmt);
				osDelay(500);
				FndAllOff();
				osDelay(500);
				FndDispalyParameter(prmt);
				DataManager(CmdWriteEeprom,CurrentModel,prmt);
				IsEditStatus=EDIT_OFF;
				IsRunMode = EDIT;
				break;
			}
		}else{
			if(keyDelayCnt>0){
				//printf(" Parameter%d Not Saved\n",prmt);
				FndDispalyParameter(prmt);
				IsEditStatus=EDIT_OFF;
				IsRunMode = EDIT;
				break;
			}
		}
		osDelay(SaveTime);
	}
	return 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	UpDateEncoder();
	IsRotaryEncoderStatus = ENCODER_RUN;
}




void Callback01(void *argument)
{


}
