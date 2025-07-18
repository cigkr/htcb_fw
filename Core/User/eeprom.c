/* | -------------------------------- R2T Team Libraries -------------------------
 * | @Created On Sep,11,2015
 * | @File Name : at24_hal_i2c
 * | @Brief : STM32 HAL Driver for AT24 eeprom series
 * | @Copyright :
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * | @Author :  Sina Darvishi
 * | @Website : R2T.IR
 * |
**/
/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"

#define TIME_OUT 	2000
#define ERR_CNT		1000
/*
 * 2018/12/08 Test�� �߰�
 *
 *at24c16 ���� ������ �޸� ���̸�  8bit ��巹�� �ּ� ũ�⸦ ������ ������, at24c128�̻��� ���� ��巹�� ũ�Ⱑ 16bit �� �Ǿ�� �Ѵ�.
 */

//Address size
#define I2C_MEMADD_SIZE16

#ifdef I2C_MEMADD_SIZE16
#define MemAddressSize I2C_MEMADD_SIZE_16BIT
#else  //I2C_MEMADD_SIZE8
#define MemAddressSize I2C_MEMADD_SIZE_8BIT
#endif



/**
  * @brief               : This function handles Writing Array of Bytes on the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	DevAddress   : specifies the slave address to be programmed(EEPROM ADDRESS).
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA WRITE TO)
  * @param	pData	     : Pointer to data buffer
  * @param  TxBufferSize : Amount of data you wanna Write
  * @retval
  */
int at24_HAL_WriteBytes(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t TxBufferSize)
{
	/*
	 * program just get the DevAddress of the Slave (not master) and for the next step
	 * You know that the most of the EEprom address start with 0xA0
	 * give MemAddress for the location you want to write to
	 * give Data buffer so it can write Data on this location
	 */
	//Note that this function works properly to 31 bytes

	while( (TxBufferSize-16)>0 )
	{
		//if your data is more than 16 bytes,you are here
		 while(HAL_I2C_Mem_Write(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,MemAddressSize,pData,(uint16_t)16,TIME_OUT)!= HAL_OK);//I2C_MEMADD_SIZE_8BIT
		 TxBufferSize-=16;
		 pData+=16;
		 MemAddress+=16;
	}
	//remaining data
	while(HAL_I2C_Mem_Write(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,MemAddressSize,pData,(uint16_t)TxBufferSize,TIME_OUT)!= HAL_OK);

	return 1;
}


int at24_HAL_ReadBytes(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t RxBufferSize)
{
	int errCnt;
	/*
	 * program just get the DevAddress of the Slave (not master) and for the next step
	 * You know that the most of the EEprom address start with 0xA0
	 * get the MemAddress for the location you want to write data on it
	 * get the Data buffer so it can write Data on this location
	 */
	//Note that this function works properly to 31bytes

	while( (RxBufferSize-16)>0 )
	{
		//if your data is more than 16 bytes,you are here
		errCnt = 0;
		while(HAL_I2C_Mem_Read(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,MemAddressSize,pData,(uint16_t)16,TIME_OUT)!= HAL_OK && errCnt < ERR_CNT){
			errCnt++;
		}
		RxBufferSize-=16;
		pData+=16;
		MemAddress+=16;
	}
	//remaining data
	errCnt = 0;
	while(HAL_I2C_Mem_Read(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,MemAddressSize,pData,(uint16_t)RxBufferSize,TIME_OUT)!= HAL_OK && errCnt < ERR_CNT){
		errCnt++;
	}

	return 1;
}

 /*
  * @brief               : This function handles Reading Array of Bytes from the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	DevAddress   : specifies the slave address to be programmed(EEPROM ADDRESS).
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA READ FROM)
  * @param	pData	     : Pointer to data buffer
  * @param  TxBufferSize : Amount of data to be Read
  * @retval
  */
int at24_HAL_SequentialRead(I2C_HandleTypeDef *hi2c ,uint8_t DevAddress,uint16_t MemAddress,uint8_t *pData,uint16_t RxBufferSize)
{
	/*
	 * just like WriteByte but get what it want
	 * but maybe you should know that the Data is location you want to save data
	 * for future use
	 */
		while( (RxBufferSize-16)>0)
		{
			while(HAL_I2C_Mem_Read(hi2c, (uint16_t)DevAddress,(uint16_t)MemAddress,MemAddressSize,pData, (uint16_t)16, (uint32_t)TIME_OUT)!= HAL_OK);
			RxBufferSize-=16;
			pData+=16;
			MemAddress+=16;
		}
		while(HAL_I2C_Mem_Read(hi2c, (uint16_t)DevAddress,(uint16_t)MemAddress,MemAddressSize,pData, (uint16_t)RxBufferSize, (uint32_t)TIME_OUT)!= HAL_OK);
        /*
         * if DataRecive  is 0xFF or 255 ,this means that block was empty
         */
	   return 1;
}

 /*
  * @brief               : This function handles Erase Full chip.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @retval
  */

int at24_HAL_EraseMemFull(I2C_HandleTypeDef *hi2c)
{
	/*
	 * this may take will don't panic
	 */
	uint8_t EraseBuf[16] ={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	int i;
	for (i =0 ; i<1024 ; i+=16)
	{
		/*
		 * if you know,0xFF means that block is empty
		 */
		at24_HAL_WriteBytes(hi2c,0xA0,(uint16_t )i,EraseBuf,16);
	}
	return 1;
}

/**
  * @brief               : This function handles Writing String on the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	DevAddress   : specifies the slave address to be programmed(EEPROM ADDRESS).
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA Write)
  * @param	pString	     : Pointer to data buffer(CHAR DATA)
  * @param  length       : Amount of buffer you wanna Write from
  * @retval
  */

int at24_HAL_WriteString(I2C_HandleTypeDef *hi2c,char *pString ,uint16_t MemAddress ,uint8_t length)
{
	uint8_t pData[length];
	int i =0;
	while(*pString)
		(pData[i++])=(uint8_t)(*pString++);
	at24_HAL_WriteBytes(hi2c,0xA0,MemAddress,pData,length);
	return 1;
}

/**
  * @brief               : This function handles Reading String on the specific Address .
  * 					   This program have this feature that don't force you to use absolute 16 bytes
  * 					   you can use more than 16 bytes buffer.
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA READ)
  * @param	pString	     : Pointer to data buffer(CHAR DATA)
  * @param  length       : Amount of buffer you wanna Read from
  * @retval
  */

int at24_HAL_ReadString(I2C_HandleTypeDef *hi2c,char *pString,uint16_t MemAddress,uint8_t length)
{
	uint8_t pData[length];
	int i=0;
	at24_HAL_ReadBytes(hi2c,0xA0,MemAddress,pData,length);
	while(pData[i])
		(*pString++)=(char)pData[i++];
	return 1;
}

/*word Write */
int at24_HAL_WriteWords(I2C_HandleTypeDef *hi2c,uint16_t *pidata ,uint16_t MemAddress,uint8_t length)
{
	uint8_t pData[length*2];
	uint8_t upperData,lowerData;
	uint8_t i=0;
	while(i<(length*2))
	{
		upperData=(*pidata & 0xff00)>>8;
		lowerData=(*pidata & 0x00ff);
		pidata++;
		(pData[i++])= upperData;
		(pData[i++])= lowerData;
	}
	at24_HAL_WriteBytes(hi2c,0xA0,MemAddress*2,pData,length*2);
	return 1;
}

/**
  * @brief               : at24_HAL_ReadWords
  * @param  hi2c         : Pointer to a I2C_HandleTypeDef structure that contains
  *                        the configuration information for the specified I2C.
  * @param  psdata       : Pointer to data buffer(uint16_t)
  * @param	MemAddress   : Internal memory address (WHERE YOU WANNA READ)
  * @param  length       : Amount of buffer you wanna Read from
  * @retval
  */
int at24_HAL_ReadWords(I2C_HandleTypeDef *hi2c, uint16_t *psdata, uint16_t MemAddress, uint8_t length)
{
	uint8_t pData[length*2];
	uint8_t upperData,lowerData;
	uint8_t i=0;
	at24_HAL_ReadBytes(hi2c,0xA0,MemAddress*2,pData,length*2);
	while(i<(length*2))
	{
		upperData = pData[i++];
		lowerData = pData[i++];
	 	(*psdata++)=(upperData<<8)|lowerData;
	}
	return 1;
}


/*Float Write*/
int at24_HAL_WriteFloat(I2C_HandleTypeDef *hi2c,float *pfdata ,uint16_t MemAddress,uint8_t length)
{
	uint8_t pData[length];

	memset(pData,0,sizeof(pData));
	sprintf((char *)pData,"%4.2f",*pfdata);  //####.##'\0' = 8byte Write
	at24_HAL_WriteBytes(hi2c,0xA0,MemAddress,pData,8);

	return 1;
}


int at24_HAL_ReadFloat(I2C_HandleTypeDef *hi2c,float *pfdata,uint16_t MemAddress,uint8_t length)
{
	uint8_t pData[length];
	memset(pData,0,sizeof(pData));
	at24_HAL_ReadBytes(hi2c,0xA0,MemAddress,pData,length);
	*pfdata=atof((char *)pData);
	return 1;
}










