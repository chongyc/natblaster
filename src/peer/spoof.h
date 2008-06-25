/*****************************************************************************
 * Copyright 2005 Daniel Ferullo                                             *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *    http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 *                                                                           *
 *****************************************************************************/

/**
 * @file spoof.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief functions to spoof/forge tcp/ip packets
 */

#ifndef __SPOOF_H__
#define __SPOOF_H__

#include "errorcodes.h"
#include "def.h"

/**
 * @brief spoofs a tcp packet
 *
 * @param tcp_hdr the tcp_packet_info_t with the essential information to spoof  *        a tcp packet based on.
 * @param device the device to spoof on
 * @param payload pointer to the payload of the packet. if NULL then no payload
 * @param payload_len the length of the payload
 * @param ttl the TTL to use on the spoofed packets
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode spoof(tcp_packet_info_t *tcp_hdr, char *device, void *payload,
					unsigned long payload_len, short ttl);

#endif /* __SPOOF_H__ */

