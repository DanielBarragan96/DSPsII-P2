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

//Limits of numbers characters
#define ARRAY_SIZE_BUF 255
#define ARRAY_NUMBER 3
#define SONG_MIN_VAL 50

//Sincronization variables
SemaphoreHandle_t mutex_audio;
TimerHandle_t g_timer;
//system variables
uint8_t counter = 0;
uint16_t newbuf[ARRAY_SIZE_BUF] = {0};
//port of UDP, default value to 50007
uint16_t port = 50007;
//flags to control thread flow
volatile static bool pit_flag = false;
volatile static bool stop_udp = false;
volatile static bool changePort = true;
volatile static bool stop_dac = false;

//This function is called when the last UDP package was received 700 mseg before.
void TimerCallback (TimerHandle_t timeIn)
{
    stop_dac = true;
}

//This function is called whenever the PIT has an interruption
void PIT0_IRQHandler()
{
    //Clear interruption flag
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    //if we want to stop sending any audio
    if(stop_udp || stop_dac)
    {//send 0 trought the DAC
        DAC_SetBufferValue(DAC0, 0U,0);
    }
    else
    {//if the value is more than 50 (reduce  noise)
        if(SONG_MIN_VAL < newbuf[counter])//send value throught the DAC
            DAC_SetBufferValue(DAC0, 0U,(newbuf[counter]));
    //increse counter of the audio
    counter = (counter < (ARRAY_SIZE_BUF - ARRAY_NUMBER )) ? counter + 1 : 0;
    }
}

//Change the song from pause to play and vice versa
void toogleUDP()
{
    stop_udp = (stop_udp)? false : true;
}
//update UDP port
void changePortNum(uint16_t newPort)
{
    port = newPort;
    //indicate that we want to change the flag
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
    //create new connection
    conn = netconn_new(NETCONN_UDP);
    //start the pit to keep sending audio trought the DAC
    PIT_StartTimer(PIT, kPIT_Chnl_0);
    while (1)
    {   
        //Start timer to check if the UDP stop sending audio
        xTimerStart(g_timer, portMAX_DELAY);
        //if we wat toupdate the UDP port
        if(changePort)
        {//bind to new port
            netconn_bind(conn, IP_ADDR_ANY, port);
            //deactivate flag
            changePort = false;
        }
        //wait for info sent
        netconn_recv(conn, &buf);
        netbuf_data(buf, (void**)&msg, &len);
        //store audio received
        netbuf_copy(buf, newbuf, sizeof(newbuf));
        //reset audio counter
        counter = 0;
        //stop timer to indicate the system received new audio
        xTimerStop(g_timer, portMAX_DELAY);
        //keep sending info trought the DAC
        stop_dac = false;
        //delete buffer
        netbuf_delete(buf);
    }
}

void
udpecho_init(void)
{
    //create mutex
    mutex_audio = xSemaphoreCreateMutex();
    //start mutex in signilized status
    xSemaphoreGive(mutex_audio);

    //Initialize system timer to check UDP sending status
    const TickType_t g_xTimerPeriod = pdMS_TO_TICKS(700); //periodo a interrumpir
    //Interrupt I2C nw
    const char *pcTimerName = "Timer";    //nombre
    const UBaseType_t uxAutoReload = pdFALSE;    //si se hace auto reload
    void * const pvTimerID = NULL; //handle de las tareas, regresa un valor para identificar la tarea
    TimerCallbackFunction_t pxCallbackFunction = TimerCallback; //callback function
    //se crea el timer, es global, tipo TimerHandle_t
    g_timer = xTimerCreate (pcTimerName, g_xTimerPeriod, uxAutoReload,
            pvTimerID, pxCallbackFunction);
    //create UDP thread
    sys_thread_new("server", server_thread, NULL, 300, 2);
}

#endif /* LWIP_NETCONN */
