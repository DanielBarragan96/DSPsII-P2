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

#include "fsl_edma.h"
#include "fsl_dmamux.h"

#include "pin_mux.h"
#include "clock_config.h"


#define EXAMPLE_DMA DMA0
#define EXAMPLE_DMAMUX DMAMUX0
#define DEMO_DMAMUX_BASEADDR DMAMUX

/*******************************************************************************
 * Variables
 ******************************************************************************/
edma_handle_t g_EDMA_Handle;             /* Edma handler */
edma_transfer_config_t g_transferConfig; /* Edma transfer config. */

edma_transfer_config_t transferConfig;

volatile bool g_Transfer_Done = false;
volatile uint32_t g_index = 0U;          /* Index of the g_dacDataArray array. */
static char songVal[9146];			 /* Array containing song values */
uint32_t srcAdress = 0x200098a2;
/*******************************************************************************
 * Code
 ******************************************************************************/

/* User callback function for EDMA transfer. */
void EDMA_Callback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
	/* Clear Edma interrupt flag. */
    EDMA_ClearChannelStatusFlags(EXAMPLE_DMA, 0U, kEDMA_InterruptFlag);
    /* Setup transfer */
    g_index += DAC_DATL_COUNT;
    srcAdress = srcAdress + 0x04;
    if (g_index == 9146)
       {
           g_index = 0U;
           srcAdress = 0x200098a2;
       }
    EDMA_PrepareTransfer(&transferConfig, (void *)srcAdress, sizeof(uint16_t), (void *)(songVal + g_index), sizeof(songVal[0]),
    			sizeof(songVal[0]), sizeof(songVal), kEDMA_MemoryToMemory);

    EDMA_SetTransferConfig(EXAMPLE_DMA, 0U, &g_transferConfig, NULL);
    /* Enable transfer. */
    EDMA_StartTransfer(&g_EDMA_Handle);
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
    EDMA_SubmitTransfer(&g_EDMA_Handle, &transferConfig);
	//LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

	while (1)
	{
		netconn_recv(conn, &buf);
		netbuf_data(buf, (void**)&msg, &len);
		EDMA_StartTransfer(&g_EDMA_Handle);
		/* Wait for EDMA transfer finish */

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
    edma_config_t userConfig;

    DMAMUX_Init(EXAMPLE_DMAMUX);
#if defined(FSL_FEATURE_DMAMUX_HAS_A_ON) && FSL_FEATURE_DMAMUX_HAS_A_ON
    DMAMUX_EnableAlwaysOn(EXAMPLE_DMAMUX, 0, true);
#else
    DMAMUX_SetSource(EXAMPLE_DMAMUX, 0, 63);
#endif /* FSL_FEATURE_DMAMUX_HAS_A_ON */
    DMAMUX_EnableChannel(EXAMPLE_DMAMUX, 0);

    EDMA_GetDefaultConfig(&userConfig);
	EDMA_Init(EXAMPLE_DMA, &userConfig);
	EDMA_CreateHandle(&g_EDMA_Handle, EXAMPLE_DMA, 0);
	EDMA_SetCallback(&g_EDMA_Handle, EDMA_Callback, NULL);
	EDMA_PrepareTransfer(&transferConfig, (void *)0x200098a2, sizeof(uint16_t), (void *)(songVal + g_index), sizeof(songVal[0]),
						 sizeof(songVal[0]), sizeof(songVal), kEDMA_MemoryToMemory);


}

#endif /* LWIP_NETCONN */
