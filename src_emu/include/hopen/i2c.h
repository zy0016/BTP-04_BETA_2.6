/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/i2c.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/08/26 02:52:24 $
 * 
\**************************************************************************/

#ifndef __HOPEN_I2C_H
#define __HOPEN_I2C_H

#define I2C_M_TEN		0x10	/* we have a ten bit chip address	*/
#define I2C_M_RD		0x01
#define I2C_M_NOSTART	0x4000
#define I2C_M_REV_DIR_ADDR	0x2000

/*
	simplified i2c bus interfaces
*/

struct i2c_client;
struct i2c_msg;

/* shall be implemented by i2c bus driver*/
extern int i2c_master_send(struct i2c_client *,const char* ,int);
extern int i2c_master_recv(struct i2c_client *,char* ,int);
extern int i2c_transfer(struct i2c_msg msg[],int num);

/* optional bus interfaces*/
extern int i2c_slave_send(struct i2c_client *,char*,int);
extern int i2c_slave_recv(struct i2c_client *,char*,int);

/*
 * i2c_client identifies a single device (i.e. chip) that is connected to an 
 * i2c bus. 
 */
struct i2c_client {
	unsigned int flags;
	unsigned int addr;		
};

struct i2c_msg
{
	unsigned short addr;	/* slave address			*/
	unsigned short flags;		
	short len;		/* msg length				*/
	char *buf;		/* pointer to msg data			*/
};

#endif /* __HOPEN_I2C_H */
