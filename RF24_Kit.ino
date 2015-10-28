// RF24_Kit.ino
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "scan.h"
#include "listen.h"
#include <stdlib.h>


String cmd = "";

#define STOP	0
#define RUNNING	1

#define JOB_NONE	0
#define JOB_SCAN	1
#define JOB_LISTEN	2

int status = STOP;
int job	= JOB_NONE;

RF24 radio(9,10);


bool getCMD()
{
	bool b = false;
	while(Serial.available() > 0)
	{
		cmd += char(Serial.read());
		delay(2);
	}

	if(cmd.length() > 0)
	{
		cmd.replace("\r", "");
		cmd.replace("\n", "");
		b = true;
	}

	return b;
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void usage()
{
	printf("RF24 Kit by AD (v1.0)\r\n\r\n");
	printf("scan	scan all channel.\r\n");
	printf("listen <channel id> <address> [rate ack crc]\r\n");
	printf("\tchannel: 1-128\r\n");
	printf("\taddress: for listen, like 0xe0e0e0e0\r\n");
	printf("\trate: value: [250, 1, 2]. default is 1m\r\n");
	printf("\tack: value: [0, 1]. 0: disable, 1: enable. default is 0\r\n");
	printf("\tcrc: value: [0, 8, 16]. 0: disable. default is 0\r\n");
	printf("\teg:listen 40 0xe0e0e0e0\r\n");
}

bool checkCMD(String test_cmd)
{
	String c = getValue(cmd, ' ', 0);
	if(c.compareTo(test_cmd) == 0)
	{
	    return true;
	}

	return false;
}

uint64_t S2Addr(String p)
{
	uint64_t r;
	char szAddr[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	char rAddr[] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	String st = p.substring(2, p.length());
	int len = st.length();

	int n = 0;
	for(int i = (len/2); i > 0; i--)
	{
		char t[2] = {st.charAt((i-1) * 2), st.charAt((i-1) * 2 + 1)};
	    szAddr[n] = strtol(t, NULL, 16);
	    szAddr[n] = szAddr[n] & 0x00FF;
		n++;
	}

/*
	for(int i=0; i<5; i++)
	{
	    printf("%02x ", szAddr[i]);
	}
*/
	r = *(long unsigned long *)szAddr;

	for(int i = 0; i < (5 - len/2); i++)
	{
		//printf("%06x ", r >> 8);
		r >> 8;
	}
	return r;
}

void setup()
{
	Serial.begin(9600);
	printf_begin();
	usage();
}

void loop() 
{
	if(getCMD())
	{
		if(checkCMD("scan"))
		{
			printf("Scan_AllChannel Start!\r\n");
			Scan_Init(&radio);

			job = JOB_SCAN;
			status = RUNNING;


		}else if(checkCMD("listen"))
		{
			//printf("listen Start!\r\n");

			String chan_id = getValue(cmd, ' ', 1);
			String addr = getValue(cmd, ' ', 2);
			int add_size = addr.length();
			String rate = getValue(cmd, ' ', 3);
			String ack = getValue(cmd, ' ', 4);
			String crc = getValue(cmd, ' ', 5);

			int channel_id = chan_id.toInt();
			int address_len = addr.length();

			//printf("chan_id: %d, addr_len:%d\r\n", channel_id, address_len);

			if(0 <= channel_id && channel_id < 127)
			{
				if(address_len >= 4 && address_len <= 12 && address_len%2 == 0 && addr.charAt(0) == '0' && addr.charAt(1) == 'x')
				{

					uint64_t u64_addr = S2Addr(addr);
					printf("listening 24%d MHz\r\n", channel_id);
					address_len = (address_len - 2)/2;
					printf("ADDR LEN: %d", address_len);
					Listen_Init(&radio, channel_id, u64_addr, address_len, rate.toInt(), ack.toInt(), crc.toInt());
					job = JOB_LISTEN;
					status = RUNNING;
				}
				else
				{
					printf("address error. eg: 0xE0E0E0E0E0");
				}

			}
			else
			{
				printf("channel id error. Range: 0 - 127");
			}


		}else if(checkCMD("stop"))
		{
			if(status == RUNNING)
			{
				if(job == JOB_SCAN)
				{
					printf("stop scan\r\n");
					radio.stopListening();
				}
				else if(job == JOB_LISTEN)
				{
					printf("stop listen\r\n");
					radio.stopListening();
				}else
				{
					printf("stop none\r\n");
				}
			    
			}
			
			job = JOB_NONE;
			status = STOP;

		}else if(checkCMD("help"))
		{
			usage();
		}else
		{
			printf("Unkown Command!\r\n");
		}


		cmd = "";
	}


	if(job == JOB_SCAN)
	{
		//printf("scaning.....\r\n");
		Scan_AllChannel();
	    
	}else if (job == JOB_LISTEN)
	{
		Listen_Recv();//printf("listening...\r\n");
	}

}

