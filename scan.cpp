#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "scan.h"

RF24 *pScan_Radio;
int v[CHS_NUM];

void Scan_Init(RF24 *p)
{
	pScan_Radio = p;
	pScan_Radio->begin();
	pScan_Radio->setAutoAck(false);
	pScan_Radio->disableCRC();
	pScan_Radio->setRetries(0,0);
	pScan_Radio->powerUp();
}


bool Scan_Channel(int channel_id)
{
	bool b = false;
	pScan_Radio->flush_rx();
	pScan_Radio->setChannel(channel_id);
	pScan_Radio->startListening();
	delayMicroseconds(128);
	pScan_Radio->stopListening();

	if (pScan_Radio->testCarrier())
	{
		b = true;
	}

	return b;
}



void Scan_AllChannel()
{
	for (int i = 0; i < CHS_NUM; ++i)
	{
		if (Scan_Channel(i))
		{
			Serial.print("*");
			v[i] = 1;
		}
		else
		{
			Serial.print(".");
			v[i] = 0;
		}
	}

	Serial.println("");

}