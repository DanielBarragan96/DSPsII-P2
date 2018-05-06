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
#include "semphr.h"
#include "timers.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

#define ARRAY_SIZE_BUF 251
#define ARRAY_NUMBER 2

SemaphoreHandle_t mutex_audio;
TimerHandle_t g_timer;

uint8_t counter = 0;
uint16_t newbuf[ARRAY_SIZE_BUF] = {0};
uint16_t newbuf2[ARRAY_SIZE_BUF] = {0};

uint16_t port = 50007;

volatile static bool pit_flag = false;
volatile static bool stop_udp = false;
volatile static bool changePort = true;
volatile static bool stop_dac = false;

void TimerCallback (TimerHandle_t timeIn)
{
    stop_dac = true;
}

void PIT0_IRQHandler()
{
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    if(stop_udp || stop_dac)
    {
        DAC_SetBufferValue(DAC0, 0U,0);
    }
    else
    {
        if(50 < newbuf[counter])
            DAC_SetBufferValue(DAC0, 0U,(newbuf[counter]));

        counter = (counter < (ARRAY_SIZE_BUF - ARRAY_NUMBER )) ? counter + 1 : 0;
    }
}


void toogleUDP()
{
    stop_udp = (stop_udp)? false : true;
}

void changePortNum(uint16_t newPort)
{
    port = newPort;
    changePort = true;
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

    PIT_StartTimer(PIT, kPIT_Chnl_0);
    while (1)
    {
        xTimerStart(g_timer, portMAX_DELAY);
        if(changePort)
        {
            netconn_bind(conn, IP_ADDR_ANY, port);
            changePort = false;
        }

        netconn_recv(conn, &buf);
        netbuf_data(buf, (void**)&msg, &len);


        netbuf_copy(buf, newbuf, sizeof(newbuf));

        counter = 0;
        xTimerStop(g_timer, portMAX_DELAY);
        stop_dac = false;
        netbuf_delete(buf);

    }
}

/*-----------------------------------------------------------------------------------*/
void
udpecho_init(void)
{
    //create mutex
    mutex_audio = xSemaphoreCreateMutex();
    //start mutex in signilized status
    xSemaphoreGive(mutex_audio);

    const TickType_t g_xTimerPeriod = pdMS_TO_TICKS(700); //periodo a interrumpir
    //Interrupt I2C nw
    const char *pcTimerName = "Timer";    //nombre
    const UBaseType_t uxAutoReload = pdFALSE;    //si se hace auto reload
    void * const pvTimerID = NULL; //handle de las tareas, regresa un valor para identificar la tarea
    TimerCallbackFunction_t pxCallbackFunction = TimerCallback; //callback function
    //se crea el timer, es global, tipo TimerHandle_t
    g_timer = xTimerCreate (pcTimerName, g_xTimerPeriod, uxAutoReload,
            pvTimerID, pxCallbackFunction);
    sys_thread_new("server", server_thread, NULL, 300, 2);
}

#endif /* LWIP_NETCONN */
