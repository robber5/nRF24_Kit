#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "listen.h"

RF24 *pListen_Radio;

void Listen_Init(RF24 *p, int chan_id, const uint64_t addr, int addr_size, int rate, int ack, int crc)
{
	pListen_Radio = p;
	pListen_Radio->begin();

	if (ack == 1)
	{
		pListen_Radio->setAutoAck(true);
	}
	else
	{
		pListen_Radio->setAutoAck(ack);	
	}

	if (crc == 8)
	{
		pListen_Radio->setCRCLength(RF24_CRC_8);
	}
	else if(crc == 16)
	{
		pListen_Radio->setCRCLength(RF24_CRC_16);
	}
	else
	{
		pListen_Radio->disableCRC();
	}

	if (rate == 250)
	{
		pListen_Radio->setDataRate(RF24_2MBPS);
	}
	else if (rate == 2)
	{
		pListen_Radio->setDataRate(RF24_2MBPS);
	}
	else
	{
		pListen_Radio->setDataRate(RF24_1MBPS);
	}

	
	pListen_Radio->setRetries(0,0);
	pListen_Radio->powerUp();

	pListen_Radio->openReadingPipe(1,addr);
	pListen_Radio->openWritingPipe(  addr);

	pListen_Radio->setAddressSize(addr_size);

	pListen_Radio->setChannel(chan_id);

	pListen_Radio->flush_rx();

	pListen_Radio->startListening();

	pListen_Radio->printDetails();
}


void Listen_Recv()
{
	char receive_payload[33]; // +1 to allow room for a terminating NULL char

	if (pListen_Radio->available())
	{
		uint8_t len;
		bool done = false;
		len = pListen_Radio->getDynamicPayloadSize();
		done = pListen_Radio->read(receive_payload, len);

		for (int i = 0; i < len; ++i)
		{
			printf("%02X ", receive_payload[i] & 0x000000ff);
		}
		printf("\r\n");
/*
		if (checkAddr(receive_payload, len))
		{
			printf("Found Addr [%d]: ", channel_id);
			for (int i = 0; i < len; ++i)
			{
				printf("%02X ", receive_payload[i] & 0x000000ff);
			}
			Serial.println("");
		}
*/
	}


}