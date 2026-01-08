/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file    mpu6050.h
  * @author  YDX 2244907035@qq.com
  * @brief   Code for MPU6050.
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version Number, write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  *
  * <h2><center>&copy; Copyright (c) 2019 - ~, SCUT-RobotLab Development Team.
  * All rights reserved.</center></h2>
  ******************************************************************************
  */ 
#ifndef __MPU6050_H
#define __MPU6050_H

#ifdef __cplusplus
 extern "C" {
#endif
     
/* Includes ------------------------------------------------------------------*/ 
#include "Drivers/Components/drv_i2c.h"
     
/* Private  ------------------------------------------------------------------*/          
#define MPU_SELF_TESTX_REG		0X0D	//!<妫€瀵勫瓨鍣╔
#define MPU_SELF_TESTY_REG		0X0E	//!<鑷�妫€瀵勫瓨鍣╕
#define MPU_SELF_TESTZ_REG		0X0F	//!<鑷�妫€瀵勫瓨鍣╖
#define MPU_SELF_TESTA_REG		0X10	//!<鑷�妫€瀵勫瓨鍣ˋ
#define MPU_SAMPLE_RATE_REG		0X19	//!<閲囨牱棰戠巼鍒嗛�戝櫒
#define MPU_CFG_REG				    0X1A	//!<閰嶇疆瀵勫瓨鍣�
#define MPU_GYRO_CFG_REG		  0X1B	//!<闄€铻轰华閰嶇疆瀵勫瓨鍣�
#define MPU_ACCEL_CFG_REG		  0X1C	//!<鍔犻€熷害璁￠厤缃�瀵勫瓨鍣�
#define MPU_MOTION_DET_REG		0X1F	//!<杩愬姩妫€娴嬮榾鍊艰�剧疆瀵勫瓨鍣�
#define MPU_FIFO_EN_REG			  0X23	//!<FIFO浣胯兘瀵勫瓨鍣�
#define MPU_I2CMST_CTRL_REG		0X24	//!<IIC涓绘満鎺у埗瀵勫瓨鍣�
#define MPU_I2CSLV0_ADDR_REG	0X25	//!<IIC浠庢満0鍣ㄤ欢鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV0_REG			  0X26	//!<IIC浠庢満0鏁版嵁鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV0_CTRL_REG	0X27	//!<IIC浠庢満0鎺у埗瀵勫瓨鍣�
#define MPU_I2CSLV1_ADDR_REG	0X28	//!<IIC浠庢満1鍣ㄤ欢鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV1_REG			  0X29	//!<IIC浠庢満1鏁版嵁鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV1_CTRL_REG	0X2A	//!<IIC浠庢満1鎺у埗瀵勫瓨鍣�
#define MPU_I2CSLV2_ADDR_REG	0X2B	//!<IIC浠庢満2鍣ㄤ欢鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV2_REG			  0X2C	//!<IIC浠庢満2鏁版嵁鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV2_CTRL_REG	0X2D	//!<IIC浠庢満2鎺у埗瀵勫瓨鍣�
#define MPU_I2CSLV3_ADDR_REG	0X2E	//!<IIC浠庢満3鍣ㄤ欢鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV3_REG			  0X2F	//!<IIC浠庢満3鏁版嵁鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV3_CTRL_REG	0X30	//!<IIC浠庢満3鎺у埗瀵勫瓨鍣�
#define MPU_I2CSLV4_ADDR_REG	0X31	//!<IIC浠庢満4鍣ㄤ欢鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV4_REG			  0X32	//!<IIC浠庢満4鏁版嵁鍦板潃瀵勫瓨鍣�
#define MPU_I2CSLV4_DO_REG		0X33	//!<IIC浠庢満4鍐欐暟鎹�瀵勫瓨鍣�
#define MPU_I2CSLV4_CTRL_REG	0X34	//!<IIC浠庢満4鎺у埗瀵勫瓨鍣�
#define MPU_I2CSLV4_DI_REG		0X35	//!<IIC浠庢満4璇绘暟鎹�瀵勫瓨鍣�

#define MPU_I2CMST_STA_REG		0X36	//!<IIC涓绘満鐘舵€佸瘎瀛樺櫒
#define MPU_INTBP_CFG_REG		  0X37	//!<涓�鏂�/鏃佽矾璁剧疆瀵勫瓨鍣�
#define MPU_INT_EN_REG			  0X38	//!<涓�鏂�浣胯兘瀵勫瓨鍣�
#define MPU_INT_STA_REG			  0X3A	//!<涓�鏂�鐘舵€佸瘎瀛樺櫒

#define MPU_ACCEL_XOUTH_REG		0X3B	//!<鍔犻€熷害鍊�,X杞撮珮8浣嶅瘎瀛樺櫒
#define MPU_ACCEL_XOUTL_REG		0X3C	//!<鍔犻€熷害鍊�,X杞翠綆8浣嶅瘎瀛樺櫒
#define MPU_ACCEL_YOUTH_REG		0X3D	//!<鍔犻€熷害鍊�,Y杞撮珮8浣嶅瘎瀛樺櫒
#define MPU_ACCEL_YOUTL_REG		0X3E	//!<鍔犻€熷害鍊�,Y杞翠綆8浣嶅瘎瀛樺櫒
#define MPU_ACCEL_ZOUTH_REG		0X3F	//!<鍔犻€熷害鍊�,Z杞撮珮8浣嶅瘎瀛樺櫒
#define MPU_ACCEL_ZOUTL_REG		0X40	//!<鍔犻€熷害鍊�,Z杞翠綆8浣嶅瘎瀛樺櫒

#define MPU_TEMP_OUTH_REG		  0X41	//!<娓╁害鍊奸珮鍏�浣嶅瘎瀛樺櫒
#define MPU_TEMP_OUTL_REG		  0X42	//!<娓╁害鍊间綆8浣嶅瘎瀛樺櫒

#define MPU_GYRO_XOUTH_REG		0X43	//!<闄€铻轰华鍊�,X杞撮珮8浣嶅瘎瀛樺櫒
#define MPU_GYRO_XOUTL_REG		0X44	//!<闄€铻轰华鍊�,X杞翠綆8浣嶅瘎瀛樺櫒
#define MPU_GYRO_YOUTH_REG		0X45	//!<闄€铻轰华鍊�,Y杞撮珮8浣嶅瘎瀛樺櫒
#define MPU_GYRO_YOUTL_REG		0X46	//!<闄€铻轰华鍊�,Y杞翠綆8浣嶅瘎瀛樺櫒
#define MPU_GYRO_ZOUTH_REG		0X47	//!<闄€铻轰华鍊�,Z杞撮珮8浣嶅瘎瀛樺櫒
#define MPU_GYRO_ZOUTL_REG		0X48	//!<闄€铻轰华鍊�,Z杞翠綆8浣嶅瘎瀛樺櫒

#define MPU_I2CSLV0_DO_REG		0X63	//!<IIC浠庢満0鏁版嵁瀵勫瓨鍣�
#define MPU_I2CSLV1_DO_REG		0X64	//!<IIC浠庢満1鏁版嵁瀵勫瓨鍣�
#define MPU_I2CSLV2_DO_REG		0X65	//!<IIC浠庢満2鏁版嵁瀵勫瓨鍣�
#define MPU_I2CSLV3_DO_REG		0X66	//!<IIC浠庢満3鏁版嵁瀵勫瓨鍣�

#define MPU_I2CMST_DELAY_REG	0X67	//!<IIC涓绘満寤舵椂绠＄悊瀵勫瓨鍣�
#define MPU_SIGPATH_RST_REG		0X68	//!<淇″彿閫氶亾澶嶄綅瀵勫瓨鍣�
#define MPU_MDETECT_CTRL_REG	0X69	//!<杩愬姩妫€娴嬫帶鍒跺瘎瀛樺櫒
#define MPU_USER_CTRL_REG		  0X6A	//!<鐢ㄦ埛鎺у埗瀵勫瓨鍣�
#define MPU_PWR_MGMT1_REG		  0X6B	//!<鐢垫簮绠＄悊瀵勫瓨鍣�1
#define MPU_PWR_MGMT2_REG		  0X6C	//!<鐢垫簮绠＄悊瀵勫瓨鍣�2 
#define MPU_FIFO_CNTH_REG		  0X72	//!<FIFO璁℃暟瀵勫瓨鍣ㄩ珮鍏�浣�
#define MPU_FIFO_CNTL_REG		  0X73	//!<FIFO璁℃暟瀵勫瓨鍣ㄤ綆鍏�浣�
#define MPU_FIFO_RW_REG			  0X74	//!<FIFO璇诲啓瀵勫瓨鍣�
#define MPU_DEVICE_ID_REG		  0X75	//!<鍣ㄤ欢ID瀵勫瓨鍣�
 
//濡傛灉AD0鑴�(9鑴�)鎺ュ湴,IIC鍦板潃涓�0X68(涓嶅寘鍚�鏈€浣庝綅).
//濡傛灉鎺�V3.3,鍒橧IC鍦板潃涓�0X69(涓嶅寘鍚�鏈€浣庝綅).
#define MPU_ADDR				0x68

/* Private type --------------------------------------------------------------*/	 

/* Exported ------------------------------------------------------------------*/
unsigned char MPU_Init(IIC_PIN_Typedef *hiic); 								//!<鍒濆�嬪寲MPU6050
void MPU_Get_Gyroscope_Init(IIC_PIN_Typedef * hiic,float *gxoffset, float *gyoffset, float *gzoffset);          //!<鍒濆�嬪寲锛岃幏鍙栭浂椋�

unsigned char MPU_Set_Gyro_Fsr(IIC_PIN_Typedef *hiic,unsigned char fsr);
unsigned char MPU_Set_Accel_Fsr(IIC_PIN_Typedef *hiic,unsigned char fsr);
unsigned char MPU_Set_LPF(IIC_PIN_Typedef *hiic,unsigned short int lpf);
unsigned char MPU_Set_Rate(IIC_PIN_Typedef *hiic,unsigned short int rate);
unsigned char MPU_Set_Fifo(IIC_PIN_Typedef *hiic,unsigned char sens);

short MPU_Get_Temperature(IIC_PIN_Typedef *hiic);
unsigned char MPU_Get_Gyroscope(IIC_PIN_Typedef *hiic,short *gx,short *gy,short *gz);       
unsigned char MPU_Get_Accelerometer(IIC_PIN_Typedef *hiic,short *ax,short *ay,short *az);   


#ifdef __cplusplus
}
#endif

#endif
/************************ COPYRIGHT SCUT-ROBOTLAB *****END OF FILE*************/
