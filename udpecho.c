/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "udpecho.h"
#include "stdlib.h"
#include "lwip/opt.h"
#include "fsl_pit.h"
#include "fsl_dac.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

#include "FreeRTOS.h"
#include "task.h"

#define ARRAY_SIZE_BUF 225
#define ARRAY_NUMBER 6

uint8_t counter = 0;
uint16_t newbuf[ARRAY_SIZE_BUF] = {0};
uint16_t newbuf2[ARRAY_SIZE_BUF] = {0};
uint16_t newbuf3[ARRAY_SIZE_BUF] = {0};
uint16_t newbuf4[ARRAY_SIZE_BUF] = {0};

bool pit_flag = false;
bool stop_udp = false;

void PIT0_IRQHandler()
{
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    if(!stop_udp)
    {
        DAC_SetBufferValue(DAC0, 0U,(newbuf[counter]));
        DAC_SetBufferValue(DAC0, 0U,(newbuf2[counter+1]));
        DAC_SetBufferValue(DAC0, 0U,(newbuf2[counter+2]));
        DAC_SetBufferValue(DAC0, 0U,(newbuf2[counter+3]));

        counter = (counter < (ARRAY_SIZE_BUF  - ARRAY_NUMBER)) ? counter + 1 : 0;
    }
}


void stopUDP()
{
    stop_udp = (stop_udp)? false : true;
}

static void
server_thread(void *arg)
{
	struct netconn *conn;
	struct netbuf *buf;

	char *msg;

	uint16_t len;

	LWIP_UNUSED_ARG(arg);
	conn = netconn_new(NETCONN_UDP);
	netconn_bind(conn, IP_ADDR_ANY, 50007);
	//LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

	while (1)
	{
		netconn_recv(conn, &buf);
		netbuf_data(buf, (void**)&msg, &len);
		//if(false == pit_flag)
			netbuf_copy(buf, newbuf, sizeof(newbuf));
			netbuf_copy(buf, newbuf2, sizeof(newbuf));
			netbuf_copy(buf, newbuf3, sizeof(newbuf));
			netbuf_copy(buf, newbuf4, sizeof(newbuf));


		PIT_StartTimer(PIT, kPIT_Chnl_0);
		pit_flag = (pit_flag == true)?false:true;
		netbuf_delete(buf);

	}
}

/*-----------------------------------------------------------------------------------*/
void
udpecho_init(void)
{
	sys_thread_new("server", server_thread, NULL, 300, 2);
}

#endif /* LWIP_NETCONN */
