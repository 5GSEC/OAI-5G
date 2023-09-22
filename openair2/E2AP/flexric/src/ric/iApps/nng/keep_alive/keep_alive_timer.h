/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */



#ifndef KEEP_ALIVE_TIMER_H
#define KEEP_ALIVE_TIMER_H 

#include <stdint.h>

//#include "../msgs/xapp_msgs.h"

/*
 * Naive timer that expires every exp_time_ms and calls the function fun(void* data) 
 */



void init_keep_alive_timer(uint32_t exp_time_ms, void (*fun)(void*), void* data);

void stop_keep_alive_timer(void);


#endif

