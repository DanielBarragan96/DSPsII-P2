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

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

#include "FreeRTOS.h"
#include "task.h"

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
	uint16_t total = 0;
	uint16_t *p;
	p = &total;

	while (1)
	{
		netconn_recv(conn, &buf);
		netbuf_data(buf, (void**)&msg, &len);

		total = 0;
		uint16_t val_0 = *(msg) -48;
		uint16_t val_1 = *(msg+1) -48;
		uint16_t val_2 = *(msg+2) -48;
		uint16_t val_3 = *(msg+3) -48;
		uint16_t val_4 = *(msg+4) -48;

		if(val_0<10)
			total = val_0 + (total*10);
		if(val_1<10)
			total = val_1 + (total*10);
		if(val_2<10)
			total = val_2 + (total*10);
		if(val_3<10)
			total = val_3 + (total*10);
		if(val_4<10)
			total = val_4 + (total*10);

		float totalf = (float) total/32797;

		totalf += 1;

		netbuf_delete(buf);

	}
}

/*-----------------------------------------------------------------------------------*/
//static void
//client_thread(void *arg)
//{
//	ip_addr_t dst_ip;
//	struct netconn *conn;
//	struct netbuf *buf;
//
//	LWIP_UNUSED_ARG(arg);
//	conn = netconn_new(NETCONN_UDP);
//	//LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);
//
//	char *msg = "Hello loopback!";
//	buf = netbuf_new();
//	netbuf_ref(buf,msg,10);
//
//	IP4_ADDR(&dst_ip, 127, 0, 0, 1);
//
//	while (1)
//	{
//		netconn_sendto(conn, buf, &dst_ip, 50000);
//		vTaskDelay(1000);
//	}
//}
/*-----------------------------------------------------------------------------------*/
void
udpecho_init(void)
{
//	sys_thread_new("client", client_thread, NULL, 300, 1);
	sys_thread_new("server", server_thread, NULL, 300, 2);

}

#endif /* LWIP_NETCONN */
