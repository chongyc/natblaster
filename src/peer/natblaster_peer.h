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
 * @file natblaster_peer.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains the peer's entrypoint into natblaster
 */

#ifndef __NATBLASTER_PEER_H__
#define __NATBLASTER_PEER_H__

#include "def.h"

/**
 * @brief the single function a peer must call to create a natblaster TCP
 *        connection
 *
 * @param helper_ip the helper's IP
 * @param helper_port the helper's port
 * @param peer_ip the peer's IP
 * @param peer_port the port the peer wants to get a TCP connection to buddy on
 * @param buddy_ext_ip the external IP address of the buddy
 * @param buddy_int_ip the internal IP address of the buddy
 * @param buddy_int_port the internal port the buddy will create a TCP
 *        connection from
 * @param device the network device to use (if NULL it is auto detected)
 * @param random FLAG_SET to indicate the peer wants to have random port
 *        allocation. This is only for development testing, and should always
 *        be FLAG_UNSET in normal use.
 *
 * @return the TCP socket, negative if failure
 */
int natblaster_connect(ip_t helper_ip, port_t helper_port, ip_t peer_ip,
		       port_t peer_port, ip_t buddy_ext_ip, ip_t buddy_int_ip,
		       port_t buddy_int_port, char *device, flag_t random);

#endif /* __NATBLASTER_PEER_H__ */

