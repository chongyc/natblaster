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
 * @file peerfsm.h
 * @author Daniel Ferullo (ferullo@cmu.edu)
 *
 * @brief contains fsm-like functions to control protocol control flow
 *
 * The convention in these functions is at function entry, receive the next
 * message, then handle it, then send a message saying what happened, then
 * call next function in fsm
 */

#ifndef __PEERFSM_H__
#define __PEERFSM_H__

#include "errorcodes.h"
#include "peerdef.h"

/**
 * @brief entry point for a peer fsm
 *
 * @param info a pointer to the conn_info_t struct with all the info
 *
 * @return SUCCESS, errorcode on failure
 */
errorcode peer_fsm_start(peer_conn_info_t *info);

#endif /* __PEERFSM_H__ */

