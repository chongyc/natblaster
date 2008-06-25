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
 * @file sniff.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief prototypes for functions that aid in sniffing packets
 */

#ifndef __SNIFF_H__
#define __SNIFF_H__

#include "errorcodes.h"
#include "peerdef.h"

/**
 * @brief finds a syn sent from the peer to the buddy, and puts it into the
 *        correct location in the peer_conn_info_t structure
 *
 * @param info pointer to the peer_conn_info_t structure
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode capture_peer_to_buddy_syn(peer_conn_info_t *info);

/**
 * @brief finds a synack that was a part of a bday flood by the buddy
 *
 * @param info a pointer to the peer_conn_info_t structure
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode capture_flooded_synack(peer_conn_info_t *info);

#endif /* __SNIFF_H__ */
