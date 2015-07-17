#include <stdio.h>                                                              
#include <stdlib.h>                                                             
#include <unistd.h>                                                             
#include <stdint.h>
#include <time.h>
                                                             
#include "wiringX.h"                                                            

#define bcd2bin(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define bin2bcd(x)	((((x) / 10) << 4) + (x) % 10)
                                                               
#define I2C_ADDR        0x51                                                  
#define HYM8563_RTC_SECTION_LEN	7  

typedef struct rtc_time {
	unsigned char tm_sec;
	unsigned char tm_min;
	unsigned char tm_hour;
	unsigned char tm_mday;
	unsigned char tm_wday;
	unsigned char tm_mon;
	int tm_year;
} rtc_time;

typedef struct rtc_alarm {
	unsigned char am_min;
	unsigned char am_hour;#include<time.h>
	unsigned char am_mday;
	unsigned char am_wday;
} rtc_alarm;         

int main(void) {   
	unsigned char i,regs[HYM8563_RTC_SECTION_LEN] = { 0, };
	unsigned char data;
	int fd_i2c;
	int k=0;

	/* date&time to be set */
	rtc_time *tm_set;
	tm_set=malloc(sizeof(struct rtc_time));
	tm_set->tm_sec=55;
	tm_set->tm_min=28;
	tm_set->tm_hour=11;
	tm_set->tm_mday=16;
	tm_set->tm_wday=4;
	tm_set->tm_mon=7;
	tm_set->tm_year=15;

	rtc_time *tm_read;
	tm_read=malloc(sizeof(struct rtc_time));

	/* alarm to be set */
	rtc_alarm *am;
	am = malloc(sizeof(struct rtc_alarm));
	am->am_min=30;
	am->am_hour=11;
	am->am_mday=16;
	am->am_wday=4;

	wiringXSetup();

	if((fd_i2c = wiringXI2CSetup(I2C_ADDR)) < 0) {                          
                fprintf(stderr, "I2C Setup failed: %i\n", fd_i2c);              
                return -1;
        } else {              #include<time.h>                                                  
                fprintf(stderr, "I2C Setup OK: %i\n", fd_i2c);                  
        }

	hym8563_read_datetime (fd_i2c, tm_read);
	hym8563_set_time(fd_i2c, tm_set);
	hym8563_read_datetime (fd_i2c, tm_read);
	//hym8563_rtc_set_alarm(fd_i2c, am);

	sleep(2);

	//set timer
	wiringXI2CWriteReg8(fd#include<time.h>_i2c, 0x0f, 0x01);//countdown
	wiringXI2CWriteReg8(fd_i2c, 0x0e, 0x83);//clcok 1/60Hz
	k=wiringXI2CReadReg8(fd_i2c, 0x01);
	//k |= 0x11;//pulse int
	k |= 0x01;
	wiringXI2CWriteReg8(fd_i2c, 0x01, k);

	//set clk_out
	wiringXI2CWriteReg8(fd_i2c, 0x0d, 0x83);
	//sleep(2);

	while(1) {
		k=wiringXI2CReadReg8(fd_i2c, 0x01);
		printf("0x01: %x\n", k);
		k=wiringXI2CReadReg8(fd_i2c, 0x0f);//read timer
		printf("0x0f: %x\n", k);
		#include<time.h>
		//wiringXI2CWriteReg8(fd_i2c, 0x01, 0x01);
		hym8563_read_datetime (fd_i2c, tm_read);
		//printf("sec: %x\n", tm_read->tm_sec);
		sleep(1);
	}
}

int hym8563_rtc_set_alarm(int fd_i2c, struct rtc_alarm *am){
	unsigned char regs[5] = {0, 0, 0, 0};
	int k=0 ,i;

	//set min
	regs[0x00] = bin2bcd(am->am_min) & 0x7f;
	wiringXI2CWriteReg8(fd_i2c, 0x09, regs[0]);
	k=wiringXI2CReadReg8(fd_i2c, 0x09);
	printf("Alarm min: %d\n", bcd2bin(k));

	//set hour
	regs[0x01] = bin2bcd(am->am_hour) & 0x7f;
	wiringXI2CWriteReg8(fd_i2c, 0x0a, *(regs+1));
	k=wiringXI2CReadReg8(fd_i2c, 0x0a);
	printf("Alarm hour: %d\n", bcd2bin(k));

	//set mouth day
	regs[0x02] = bin2bcd(am->am_mday) & 0x7f;
	wiringXI2CWriteReg8(fd_i2c, 0x0b, *(regs+2));
	k=wiringXI2CReadReg8(fd_i2c, 0x0b);
	printf("Alarm day: %d\n", bcd2bin(k));

	//set weekly day
	regs[0x03] = (am->am_wday) & 0x0f;
	printf("week: %d\n",am->am_wday);
	wiringXI2CWriteReg8(fd_i2c, 0x0c, *(regs+3));
	k=wiringXI2CReadReg8(fd_i2c, 0x0c);
	printf("Weekly alarm: %d\n", bcd2bin(k));

	//regs[4]=wiringXI2CReadReg8(fd_i2c, 0x01);
	//regs[4] = 0x02;
	k=wiringXI2CReadReg8(fd_i2c, 0x01);
	k |= 0x02;
	wiringXI2CWriteReg8(fd_i2c, 0x01, k);
	k=wiringXI2CReadReg8(fd_i2c, 0x01);
	printf("CTL1: %d\n", k);
}

int hym8563_set_time(int fd_i2c, rtc_time *tm) {
	unsigned char regs[HYM8563_RTC_SECTION_LEN] = { 0, };
	int i = 0;

	printf("Time to be set: %4d-%02d-%02d(%d) %02d:%02d:%02d\n",
		tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_wday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);

	if(tm->tm_sec >= 60 || tm->tm_sec < 0 )	{	//set  sec
		regs[0x00] = bin2bcd(0x00);
	} else {
		regs[0x00] = bin2bcd(tm->tm_sec);
	}
	wiringXI2CWriteReg8(fd_i2c, 0x02, *regs);

	if(tm->tm_min >= 60 || tm->tm_min < 0 )	{	//set  min	
		regs[0x01] = bin2bcd(0x00);
	} else {
		regs[0x01] = bin2bcd(tm->tm_min);
	}
	wiringXI2CWriteReg8(fd_i2c, 0x03, *(regs+1));

	if(tm->tm_hour >= 24 || tm->tm_hour < 0 ) {	//set  hour
		regs[0x02] = bin2bcd(0x00);
	} else {
		regs[0x02] = bin2bcd(tm->tm_hour);
	}
	wiringXI2CWriteReg8(fd_i2c, 0x04, *(regs+2));
	
	regs[0x03] = bin2bcd(tm->tm_mday);
	wiringXI2CWriteReg8(fd_i2c, 0x05, *(regs+3));

	regs[0x04] = bin2bcd(tm->tm_wday);
	wiringXI2CWriteReg8(fd_i2c, 0x06, *(regs+4));

	regs[0x05] = bin2bcd(tm->tm_mon);
	//regs[0x05] |= 0x80;
	wiringXI2CWriteReg8(fd_i2c, 0x07, *(regs+5));

	regs[0x06] = bin2bcd(tm->tm_year);
	wiringXI2CWriteReg8(fd_i2c, 0x08, *(regs+6));
}

int hym8563_read_datetime (int fd_i2c, rtc_time *tm){
	int i = 0;
	unsigned char regs[HYM8563_RTC_SECTION_LEN] = { 0, };

	for(i=0;i<HYM8563_RTC_SECTION_LEN;i++) {
		regs[i]=wiringXI2CReadReg8(fd_i2c, i+2);
		//printf("%d\n",regs[i]);
	}

	tm->tm_sec = bcd2bin(regs[0x00] & 0x7F);//取7位
	tm->tm_min = bcd2bin(regs[0x01] & 0x7F);
	tm->tm_hour = bcd2bin(regs[0x02] & 0x3F);
	tm->tm_mday = bcd2bin(regs[0x03] & 0x3F);
	tm->tm_wday = bcd2bin(regs[0x04] & 0x07);	
	tm->tm_mon = bcd2bin(regs[0x05] & 0x1F) ; 
	tm->tm_year = bcd2bin(regs[0x06] & 0xFF);

	/* check century */
	if(regs[5] & 0x80)
		tm->tm_year += 1900;
	else
		tm->tm_year += 2000;	

	printf("Time read: %4d-%02d-%02d(%d) %02d:%02d:%02d\n",
		tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_wday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}






